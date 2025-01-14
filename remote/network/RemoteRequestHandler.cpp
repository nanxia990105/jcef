#include "RemoteRequestHandler.h"
#include "../CefUtils.h"
#include "../callback/RemoteAuthCallback.h"
#include "../callback/RemoteCallback.h"
#include "../handlers/RemoteClientHandler.h"
#include "RemoteRequest.h"
#include "RemoteResourceRequestHandler.h"

std::string err2str(cef_errorcode_t errorcode);
namespace {
  std::string tstatus2str(cef_termination_status_t status);
}

// Disable logging until optimized
#define LNDCT()

RemoteRequestHandler::RemoteRequestHandler(RemoteClientHandler & owner) : myOwner(owner) {}

RemoteRequestHandler::~RemoteRequestHandler() {
  // simple protection for leaking via callbacks
  for (auto c: myCallbacks)
    RemoteCallback::dispose(c);
  for (auto c: myAuthCallbacks)
    RemoteAuthCallback::dispose(c);
}

bool RemoteRequestHandler::OnBeforeBrowse(CefRefPtr<CefBrowser> browser,
                    CefRefPtr<CefFrame> frame,
                    CefRefPtr<CefRequest> request,
                    bool user_gesture,
                    bool is_redirect
) {
  LNDCT();
  // Forward request to ClientHandler to make the message_router_ happy.
  myOwner.getRoutersManager()->OnBeforeBrowse(browser, frame);

  RemoteRequest * rr = RemoteRequest::create(myOwner.getService(), request);
  Holder<RemoteRequest> holder(*rr);
  return myOwner.exec<bool>([&](RpcExecutor::Service s){
    return s->RequestHandler_OnBeforeBrowse(myOwner.getBid(), rr->serverIdWithMap(), user_gesture, is_redirect);
  }, false);
}

bool RemoteRequestHandler::OnOpenURLFromTab(CefRefPtr<CefBrowser> browser,
                      CefRefPtr<CefFrame> frame,
                      const CefString& target_url,
                      WindowOpenDisposition target_disposition,
                      bool user_gesture
) {
  LNDCT();
  return myOwner.exec<bool>([&](RpcExecutor::Service s){
    return s->RequestHandler_OnOpenURLFromTab(myOwner.getBid(), target_url.ToString(), user_gesture);
  }, false);
}

CefRefPtr<CefResourceRequestHandler> RemoteRequestHandler::GetResourceRequestHandler(
    CefRefPtr<CefBrowser> browser,
    CefRefPtr<CefFrame> frame,
    CefRefPtr<CefRequest> request,
    bool is_navigation,
    bool is_download,
    const CefString& request_initiator,
    bool& disable_default_handling
) {
  // Called on the browser process IO thread before a resource request is initiated.
  LogNdc ndc(__FILE_NAME__, __FUNCTION__, 500, false, false, "ChromeIO");

  if (!myResourceRequestHandlerReceived) {
    RemoteRequest * rr = RemoteRequest::create(myOwner.getService(), request);
    Holder<RemoteRequest> holder(*rr);
    thrift_codegen::RObject peer;
    peer.__set_objId(-1);
    myOwner.exec([&](RpcExecutor::Service s){
      s->RequestHandler_GetResourceRequestHandler(
          peer, myOwner.getBid(), rr->serverIdWithMap(), is_navigation, is_download, request_initiator.ToString());
    });
    myResourceRequestHandlerReceived = true;
    if (!peer.__isset.isPersistent || !peer.isPersistent)
      Log::error("Non-persistent ResourceRequestHandler can cause unstable behaviour and won't be used.");
    else if (peer.objId != -1) {
      myResourceRequestHandler = new RemoteResourceRequestHandler(myOwner, peer);
      myDisableDefaultHandling = peer.__isset.isDisableDefaultHandling &&
                                 peer.isDisableDefaultHandling;
    }
  }

  disable_default_handling = myDisableDefaultHandling;
  return myResourceRequestHandler;
}

bool RemoteRequestHandler::GetAuthCredentials(CefRefPtr<CefBrowser> browser,
                        const CefString& origin_url,
                        bool isProxy,
                        const CefString& host,
                        int port,
                        const CefString& realm,
                        const CefString& scheme,
                        CefRefPtr<CefAuthCallback> callback
) {
  LNDCT();
  thrift_codegen::RObject rc = RemoteAuthCallback::create(myOwner.getService(), callback);
  const bool handled = myOwner.exec<bool>([&](RpcExecutor::Service s){
      return s->RequestHandler_GetAuthCredentials(myOwner.getBid(), origin_url.ToString(), isProxy, host.ToString(), port, realm.ToString(), scheme.ToString(), rc);
  }, false);
  if (!handled)
    RemoteAuthCallback::dispose(rc.objId);
  else
    myAuthCallbacks.insert(rc.objId);
  return handled;
}

bool RemoteRequestHandler::OnCertificateError(CefRefPtr<CefBrowser> browser,
                        cef_errorcode_t cert_error,
                        const CefString& request_url,
                        CefRefPtr<CefSSLInfo> ssl_info,
                        CefRefPtr<CefCallback> callback
) {
  LNDCT();
  thrift_codegen::RObject rc = RemoteCallback::create(myOwner.getService(), callback);
  thrift_codegen::RObject sslInfo;
  sslInfo.__set_objId(-1); // TODO: implement ssl_info
  const bool handled = myOwner.exec<bool>([&](RpcExecutor::Service s){
      return s->RequestHandler_OnCertificateError(myOwner.getBid(), err2str(cert_error), request_url, sslInfo, rc);
  }, false);
  if (!handled)
    RemoteCallback::dispose(rc.objId);
  else
    myCallbacks.insert(rc.objId);
  return handled;
}

void RemoteRequestHandler::OnRenderProcessTerminated(CefRefPtr<CefBrowser> browser, TerminationStatus status) {
  LNDCT();
  // Forward request to ClientHandler to make the message_router_ happy.
  myOwner.getRoutersManager()->OnRenderProcessTerminated(browser);
  myOwner.exec([&](RpcExecutor::Service s){
    s->RequestHandler_OnRenderProcessTerminated(myOwner.getBid(), tstatus2str(status));
  });
}

namespace {
  std::pair<cef_errorcode_t, std::string> errorcodes [] = {
      #define NET_ERROR(label, value) {ERR_##label,"ERR_"#label},
      #include "include/base/internal/cef_net_error_list.h"
      #undef NET_ERROR
      {ERR_NONE, "ERR_NONE"}
  };

  std::pair<cef_termination_status_t, std::string> terminationStatuses [] = {
      {TS_ABNORMAL_TERMINATION, "TS_ABNORMAL_TERMINATION"},
      {TS_PROCESS_WAS_KILLED, "TS_PROCESS_WAS_KILLED"},
      {TS_PROCESS_CRASHED, "TS_PROCESS_CRASHED"},
      {TS_PROCESS_OOM, "TS_PROCESS_OOM"}
  };

  std::string tstatus2str(cef_termination_status_t status) {
    for (auto p: terminationStatuses) {
      if (p.first == status)
        return p.second;
    }
    return string_format("unknown_termination_status_%d", status);
  }
}

std::string err2str(cef_errorcode_t errorcode) {
    for (auto p: errorcodes) {
      if (p.first == errorcode)
        return p.second;
    }
    Log::error("Can't find cef_errorcode_t: %d", errorcode);
    return string_format("unknown_errorcode_%d", errorcode);
}

cef_errorcode_t str2err(std::string err) {
    for (auto p: errorcodes) {
      if (p.second.compare(err) == 0)
        return p.first;
    }
    Log::error("Can't find cef_errorcode_t: %s", err.c_str());
    return ERR_NONE;
}
