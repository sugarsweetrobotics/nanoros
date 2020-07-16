#include <iostream>
#include "nanoros/rosmaster.h"


#include "XmlRpc/XmlRpc.h"


using namespace ssr::nanoros;


namespace {



template<typename T>
std::vector<T> forEach(XmlRpc::XmlRpcValue& value, std::function<T(XmlRpc::XmlRpcValue&)> func) {
  std::vector<T> buf;
  for(int i = 0;i < value.size();i++) {
    buf.emplace_back(func(value[i]));
  }
  return buf;
}

  template<typename T>
  std::vector<T> conv(XmlRpc::XmlRpcValue& value) {
    return forEach<T>
      (value,
       [](XmlRpc::XmlRpcValue& v) -> T {
	 return {v[0], forEach<std::string>
		 (v[1],
		  [](XmlRpc::XmlRpcValue& v) -> std::string {
		    return (std::string)v;
		  })};
       }
       );
  }
}


class ROSMasterImpl : public ROSMaster {
private:
  XmlRpc::XmlRpcClient client_;
  
public:
  ROSMasterImpl(const std::string& addr, const int32_t port) : client_(addr.c_str(), port) {
  }

  virtual ~ROSMasterImpl() {}


public:
  
  virtual std::optional<SystemState> getSystemState(const std::string& caller_id) {
    XmlRpc::XmlRpcValue v, result;
    v[0] = caller_id;
    if (client_.execute("getSystemState", v, result)) {
      auto pubs = conv<TopicPublishersInfo>(result[2][0]);
      auto subs = conv<TopicSubscribersInfo>(result[2][1]);;
      auto srvs = conv<ServicesInfo>(result[2][2]);
      return SystemState(result[0], result[1], {pubs, subs, srvs});
    }
    return std::nullopt;
  }

  virtual std::optional<TopicTypes>  getTopicTypes(const std::string& caller_id) {
    XmlRpc::XmlRpcValue v, result;
    v[0] = caller_id;
    if (client_.execute("getTopicTypes", v, result)) {
      return TopicTypes(result[0], result[1], forEach<TopicTypeInfo>(result[2], [](auto& v) -> TopicTypeInfo {
										  return {v[0], v[1]}; }));
    }
    return std::nullopt;
  }

};



NANOROS_API std::shared_ptr<ROSMaster> ssr::nanoros::rosmaster(const std::string& addr, const int32_t port) {
  auto rp = std::make_shared<ROSMasterImpl>(addr, port);
  return std::static_pointer_cast<ROSMaster>(rp);
}
