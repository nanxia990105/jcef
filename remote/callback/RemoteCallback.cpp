#include "RemoteCallback.h"

#include <utility>

RemoteCallback::RemoteCallback(std::shared_ptr<RpcExecutor> service, CefRefPtr<CefCallback> delegate, int id)
    : RemoteServerObject<RemoteCallback, CefCallback>(std::move(service), id, std::move(delegate)) {}

thrift_codegen::RObject RemoteCallback::create(std::shared_ptr<RpcExecutor> service, CefRefPtr<CefCallback> delegate) {
  return FACTORY.create([&](int id) -> RemoteCallback* {return new RemoteCallback(service, delegate, id);})->serverId();
}