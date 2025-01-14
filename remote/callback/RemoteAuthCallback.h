#ifndef JCEF_REMOTEAUTHCALLBACK_H
#define JCEF_REMOTEAUTHCALLBACK_H

#include "../RemoteObjects.h"
#include "include/cef_auth_callback.h"
#include "include/cef_base.h"

class RemoteAuthCallback : public virtual CefBaseRefCounted, public RemoteServerObject<RemoteAuthCallback, CefAuthCallback> {
 public:
  static thrift_codegen::RObject create(std::shared_ptr<RpcExecutor> service, CefRefPtr<CefAuthCallback> delegate);

 private:
  explicit RemoteAuthCallback(std::shared_ptr<RpcExecutor> service, CefRefPtr<CefAuthCallback> delegate, int id);
  IMPLEMENT_REFCOUNTING(RemoteAuthCallback);
};


#endif  // JCEF_REMOTEAUTHCALLBACK_H
