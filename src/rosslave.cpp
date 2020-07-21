#include "nanoros/rosslave.h"
#include "nanoros/rosutil.h"

#include "XmlRpc/XmlRpc.h"

using namespace ssr::nanoros;

class ROSSlaveImpl : public ROSSlave {
private:
  const std::string ip_;
  const int32_t port_;
  XmlRpc::XmlRpcClient client_;
public:
  ROSSlaveImpl(const std::string& ip, const int32_t port) : ip_(ip), port_(port), client_(ip.c_str(), port)  {}
  virtual ~ROSSlaveImpl() {}


  virtual std::optional<RequestTopicResult> requestTopic(const std::string& caller_id,
                const std::string& topicName,
                const std::vector<ProtocolInfo>& info) 
  { 
    XmlRpc::XmlRpcValue v, result;
    v[0] = caller_id;
    v[1] = topicName;
    v[2] = XmlRpc::XmlRpcValue();
    for(uint32_t i = 0;i < info.size();++i) {
      XmlRpc::XmlRpcValue pi;
      auto p = info[i];
      pi[0] = p.name;
      pi[1] = p.arg0;
      pi[2] = p.arg1;
      v[2][i] = pi;
    }
    if (client_.execute("requestTopic", v, result)) {
      return RequestTopicResult((int)result[0], result[1], ProtocolInfo(result[2][0], result[2][1], result[2][2]));
    }
    return std::nullopt;
  }
};

std::shared_ptr<ROSSlave> ssr::nanoros::rosslave(const std::string& publisherUri) {
  auto info = splitUri(publisherUri);
  return std::static_pointer_cast<ROSSlave>(std::make_shared<ROSSlaveImpl>(info->first, info->second));
}
