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
  virtual std::optional<MasterUriInfo> getUri(const std::string& caller_id) override {
    XmlRpc::XmlRpcValue v, result;
    v[0] = caller_id;
    if (client_.execute("getUri", v, result)) {
      return MasterUriInfo(result[0], result[1], result[2]);
    }
    return std::nullopt;
  }

  
  virtual std::optional<SystemState> getSystemState(const std::string& caller_id) override {
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

  virtual std::optional<TopicTypes>  getTopicTypes(const std::string& caller_id) override {
    XmlRpc::XmlRpcValue v, result;
    v[0] = caller_id;
    if (client_.execute("getTopicTypes", v, result)) {
      return TopicTypes(result[0], result[1], forEach<TopicTypeInfo>(result[2], [](auto& v) -> TopicTypeInfo {
										  return {v[0], v[1]}; }));
    }
    return std::nullopt;
  }

  virtual std::optional<NodeUriInfo> lookupNode(const std::string& caller_id, const std::string& node_name) override { 
    XmlRpc::XmlRpcValue v, result;
    v[0] = caller_id;
    v[1] = node_name;
    if (client_.execute("lookupNode", v, result)) {
      return NodeUriInfo(result[0], result[1], result[2]);
    }
    return std::nullopt; 
  }

  virtual std::optional<ServiceUriInfo> lookupService(const std::string& caller_id, const std::string& srv_name) override { 
    XmlRpc::XmlRpcValue v, result;
    v[0] = caller_id;
    v[1] = srv_name;
    if (client_.execute("lookupService", v, result)) {
      return ServiceUriInfo(result[0], result[1], result[2]);
    }
    return std::nullopt; 
  }

  virtual std::optional<TopicTypes> getPublishedTopics(const std::string& caller_id, const std::string& subgraph) override { 
    XmlRpc::XmlRpcValue v, result;
    v[0] = caller_id;
    v[1] = subgraph; 
    if (client_.execute("getPublishedTopics", v, result)) {
      return TopicTypes(result[0], result[1], forEach<TopicTypeInfo>(result[2], [](auto& v) -> TopicTypeInfo {
										  return {v[0], v[1]}; }));
    }
    return std::nullopt; 
  }

  virtual std::optional<PublishersInfo> registerSubscriber(const std::string& caller_id, const std::string& topicName, const std::string& topicType, const std::string& caller_api) override {
    XmlRpc::XmlRpcValue v, result;
    v[0] = caller_id;
    v[1] = topicName;
    v[2] = topicType;
    v[3] = caller_api;
    if (client_.execute("registerSubscriber", v, result)) {
      return PublishersInfo(result[0], result[1], forEach<std::string>(result[2], [](auto& v) -> std::string {
										    return std::string(v); }));
    }
    return std::nullopt;
    
  }

  virtual std::optional<UnregisterInfo> unregisterSubscriber(const std::string& caller_id, const std::string& topicName, const std::string& topicType, const std::string& caller_api) override {
    XmlRpc::XmlRpcValue v, result;
    v[0] = caller_id;
    v[1] = topicName;
    v[2] = topicType;
    v[3] = caller_api;
    if (client_.execute("unregisterSubscriber", v, result)) {
      return UnregisterInfo(result[0], result[1], result[2]);
    }
    return std::nullopt;
    
  }


  virtual std::optional<SubscribersInfo> registerPublisher(const std::string& caller_id, const std::string& topicName, const std::string& topicType, const std::string& caller_api) override { 
    XmlRpc::XmlRpcValue v, result;
    v[0] = caller_id;
    v[1] = topicName;
    v[2] = topicType;
    v[3] = caller_api;
    if (client_.execute("registerPublisher", v, result)) {
      return SubscribersInfo(result[0], result[1], forEach<std::string>(result[2], [](auto& v) -> std::string {
										    return std::string(v); }));
    }
    return std::nullopt;
  }  

  virtual std::optional<UnregisterInfo> unregisterPublisher(const std::string& caller_id, const std::string& topicName, const std::string& topicType, const std::string& caller_api) override {
    XmlRpc::XmlRpcValue v, result;
    v[0] = caller_id;
    v[1] = topicName;
    v[2] = topicType;
    v[3] = caller_api;
    if (client_.execute("unregisterPublisher", v, result)) {
      return UnregisterInfo(result[0], result[1], result[2]);
    }
    return std::nullopt;
    
  }

  virtual std::optional<MasterMsg> registerService(const std::string& caller_id, const std::string& serviceName, const std::string& serviceUri, const std::string& caller_api) override { 
    XmlRpc::XmlRpcValue v, result;
    v[0] = caller_id;
    v[1] = serviceName;
    v[2] = serviceUri;
    v[3] = caller_api;
    if (client_.execute("registerService", v, result)) {
      return MasterMsg(result[0], result[1]);
    }
    return std::nullopt; 
  }

  virtual std::optional<UnregisterInfo> unregisterService(const std::string& caller_id, const std::string& serviceName, const std::string& serviceUri, const std::string& caller_api) override { 
    XmlRpc::XmlRpcValue v, result;
    v[0] = caller_id;
    v[1] = serviceName;
    v[2] = serviceUri;
    v[3] = caller_api;
    if (client_.execute("unregisterService", v, result)) {
      return UnregisterInfo(result[0], result[1], result[2]);
    }
    return std::nullopt; 
  }

};



NANOROS_API std::shared_ptr<ROSMaster> ssr::nanoros::rosmaster(const std::string& addr, const int32_t port) {
  return std::static_pointer_cast<ROSMaster>(std::make_shared<ROSMasterImpl>(addr, port));
}
