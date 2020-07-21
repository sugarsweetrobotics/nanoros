#pragma once

#include <vector>
#include <string>
#include <optional>
#include <memory>

#include "nanoros/rosutil.h"

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

    struct PublishersInfo : public MasterMsg {
      std::vector<std::string> publishers;
      virtual ~PublishersInfo() {}
      PublishersInfo(const int32_t _code, const  std::string& _statusMessage, const std::vector<std::string>& _publishers): MasterMsg(_code, _statusMessage), publishers(_publishers) {}
    };


    struct SubscribersInfo : public MasterMsg {
      std::vector<std::string> subscribers;
      virtual ~SubscribersInfo() {}
      SubscribersInfo(const int32_t _code, const  std::string& _statusMessage, const std::vector<std::string>& _subscribers): MasterMsg(_code, _statusMessage), subscribers(_subscribers) {}
    };


    class NANOROS_API ROSMaster {
    public:
      ROSMaster() {};
      virtual ~ROSMaster() {};
    public:

      virtual std::optional<SystemState> getSystemState(const std::string& caller_id) { return std::nullopt; }
      virtual std::optional<TopicTypes>  getTopicTypes(const std::string& caller_id) { return std::nullopt; }
      virtual std::optional<PublishersInfo> registerSubscriber(const std::string& caller_id, const std::string& topicName, const std::string& topicType, const std::string& caller_api) { return std::nullopt; }
      virtual std::optional<SubscribersInfo> registerPublisher(const std::string& caller_id, const std::string& topicName, const std::string& topicType, const std::string& caller_api) { return std::nullopt; }
    };
    
    NANOROS_API std::shared_ptr<ROSMaster> rosmaster(const std::string& addr, const int32_t port);
    inline NANOROS_API std::shared_ptr<ROSMaster> rosmaster() {
      auto info = getROSMasterInfo();
      if (!info) { return std::make_shared<ROSMaster>(); }
      return rosmaster(info->first, info->second);
    }
  }
}
