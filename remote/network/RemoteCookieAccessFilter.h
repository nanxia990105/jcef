#ifndef JCEF_REMOTECOOKIEACCESSFILTER_H
#define JCEF_REMOTECOOKIEACCESSFILTER_H

#include "../RemoteObjects.h"
#include "include/cef_resource_request_handler.h"

class RemoteCookieAccessFilter : public CefCookieAccessFilter, public RemoteJavaObject<RemoteCookieAccessFilter> {
 public:
  explicit RemoteCookieAccessFilter(RemoteClientHandler& owner, thrift_codegen::RObject peer);

  bool CanSendCookie(CefRefPtr<CefBrowser> browser,
                     CefRefPtr<CefFrame> frame,
                     CefRefPtr<CefRequest> request,
                     const CefCookie& cookie) override;

  bool CanSaveCookie(CefRefPtr<CefBrowser> browser,
                     CefRefPtr<CefFrame> frame,
                     CefRefPtr<CefRequest> request,
                     CefRefPtr<CefResponse> response,
                     const CefCookie& cookie) override;

 private:
  IMPLEMENT_REFCOUNTING(RemoteCookieAccessFilter);
};

#endif  // JCEF_REMOTECOOKIEACCESSFILTER_H
