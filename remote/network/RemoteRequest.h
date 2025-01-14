#ifndef JCEF_REMOTEREQUEST_H
#define JCEF_REMOTEREQUEST_H

#include <map>
#include "../RemoteObjects.h"
#include "../Utils.h"
#include "include/cef_request.h"

class RemoteRequest : public virtual CefBaseRefCounted, public RemoteServerObject<RemoteRequest, CefRequest> {
 public:
  void updateImpl(const std::map<std::string, std::string>& requestInfo) override;
  std::map<std::string, std::string> toMapImpl() override;

  static RemoteRequest * create(std::shared_ptr<RpcExecutor> service, CefRefPtr<CefRequest> delegate);

 private:
  explicit RemoteRequest(std::shared_ptr<RpcExecutor> service, CefRefPtr<CefRequest> delegate, int id);
  IMPLEMENT_REFCOUNTING(RemoteRequest);
};

void fillMap(CefRequest::HeaderMap & out, const std::map<std::string, std::string> & in);
void fillMap(std::map<std::string, std::string> & out, const CefRequest::HeaderMap & in);

#endif  // JCEF_REMOTEREQUEST_H
