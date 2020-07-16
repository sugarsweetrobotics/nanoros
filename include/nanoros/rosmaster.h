#pragma once

#include <vector>
#include <string>
#include <optional>
#include <memory>

#define NANOROS_API



#define ROS_MASTER_DEFAULT_PORT 11311

namespace ssr {

  namespace nanoros {


    struct MasterMsg {
    public:
      const int32_t code;
      const std::string statusMessage;

    public:
      MasterMsg(const int32_t _code, const std::string& _statusMessage): code(_code), statusMessage(_statusMessage) {}
      virtual ~MasterMsg() {}

    };

    struct TopicPublishersInfo {
      const std::string topicName;
      const std::vector<std::string> publishers;
    };
    struct TopicSubscribersInfo {
      const std::string topicName;
      const std::vector<std::string> subscribers;
    };
    struct ServicesInfo {
      const std::string serviceName;
      const std::vector<std::string> services;
    };

    struct SystemState : public MasterMsg{
    public:
      struct State {
	const std::vector<TopicPublishersInfo> publishers;
	const std::vector<TopicSubscribersInfo> subscribers;
	const std::vector<ServicesInfo> services;
      public:
	/*
	State(const std::vector<TopicPublishersInfo>& pubs,
	      const std::vector<TopicSubscribersInfo>& subs,
	      const std::vector<ServicesInfo>& srvs) : publishers(pubs), subscribers(subs), services(srvs) {} */
      } const systemState;

    public:
      SystemState(const int32_t _code, const std::string& _statusMessage, const State& state) : MasterMsg(_code, _statusMessage), systemState(state) {}
      virtual ~SystemState() {}

    };

    struct TopicTypeInfo {
    public:
      const std::string topicName;
      const std::string topicType;
      TopicTypeInfo(const std::string& name, const std::string& type) : topicName(name), topicType(type) {}
    };
    
    struct TopicTypes : public MasterMsg {
    public:
      const std::vector<TopicTypeInfo> topicTypes;
    public:
      TopicTypes(const int32_t _code, const std::string& _statusMessage, const std::vector<TopicTypeInfo>& _topicTypes): MasterMsg(_code, _statusMessage), topicTypes(_topicTypes) {}
      virtual ~TopicTypes() {}
    };

    class NANOROS_API ROSMaster {
    public:
      ROSMaster() {};
      virtual ~ROSMaster() {};
    public:

      virtual std::optional<SystemState> getSystemState(const std::string& caller_id) { return std::nullopt; }
      virtual std::optional<TopicTypes>  getTopicTypes(const std::string& caller_id) { return std::nullopt; }
    };
    
    NANOROS_API std::shared_ptr<ROSMaster> rosmaster(const std::string& addr, const int32_t port);
    
  }
}
