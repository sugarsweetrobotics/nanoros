#pragma once

#include <vector>
#include <string>
#include <optional>
#include <memory>

#include "nanoros/rosutil.h"
#include "nanoros/nanoros_define.h"



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
      std::string topicName;
      std::vector<std::string> publishers;
    };
    struct TopicSubscribersInfo {
      std::string topicName;
      std::vector<std::string> subscribers;
    };
    struct ServicesInfo {
      std::string serviceName;
      std::vector<std::string> services;
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

    struct UnregisterInfo : public MasterMsg {
      int32_t numUnregistered;
      UnregisterInfo(const int32_t _code, const  std::string& _statusMessage, const int32_t numUnregister): MasterMsg(_code, _statusMessage), numUnregistered(numUnregister) {}
    };

    struct MasterUriInfo : public MasterMsg {
      std::string uri;
      MasterUriInfo(const int32_t _code, const  std::string& _statusMessage, const std::string& uri_): MasterMsg(_code, _statusMessage), uri(uri_) {}
    };

    struct NodeUriInfo : public MasterMsg {
      std::string uri;
      NodeUriInfo(const int32_t _code, const  std::string& _statusMessage, const std::string& uri_): MasterMsg(_code, _statusMessage), uri(uri_) {}
    };

    struct ServiceUriInfo : public MasterMsg {
      std::string uri;
      ServiceUriInfo(const int32_t _code, const  std::string& _statusMessage, const std::string& uri_): MasterMsg(_code, _statusMessage), uri(uri_) {}
    };

    struct PublishedTopicsInfo : public MasterMsg {
      std::vector<TopicTypeInfo> topics;
      PublishedTopicsInfo(const int32_t _code, const  std::string& _statusMessage, const std::vector<TopicTypeInfo>& topics_): MasterMsg(_code, _statusMessage), topics(topics_) {}
    };

    /**
     * Interfaces of ROSMaster API proxy object.
     * 
     * This class only gives API interface. All interfaces returns optional (Maybe) variables. 
     */
    class NANOROS_API ROSMaster {
    public:
      ROSMaster() {};
      virtual ~ROSMaster() {};
    public:

      /**
       * Get the URI of the master.
       * @param caller_id (str) ROS caller ID
       */
      virtual std::optional<MasterUriInfo> getUri(const std::string& caller_id) { return std::nullopt; }

      /**
       * Retrieve list representation of system state (i.e. publishers, subscribers, and services).
       * @param caller_id (str) ROS caller ID
       */
      virtual std::optional<SystemState> getSystemState(const std::string& caller_id) { return std::nullopt; }

      /**
       * Retrieve list topic names and their types.
       * @param caller_id (str) ROS caller ID
       */
      virtual std::optional<TopicTypes>  getTopicTypes(const std::string& caller_id) { return std::nullopt; }

      /**
       * Get the XML-RPC URI of the node with the associated name/caller_id. This API is for looking information about publishers and subscribers. Use lookupService instead to lookup ROS-RPC URIs.
       * 
       * @param caller_id (str) ROS caller ID
       * @param node (str) Name of node to lookup
       * @returns Returns (int, str, str) (code, statusMessage, URI)
       */
      virtual std::optional<NodeUriInfo> lookupNode(const std::string& caller_id, const std::string& node_name) { return std::nullopt; }

      /**
       * Lookup all provider of a particular service.
       * 
       * @param caller_id (str) ROS caller ID
       * @param service (str) Fully-qualified name of service
       * @returns Returns (int, str, str) (code, statusMessage, serviceUrl)
       */
      virtual std::optional<ServiceUriInfo> lookupService(const std::string& caller_id, const std::string& srv_name) { return std::nullopt; }

      /**
       * Get list of topics that can be subscribed to. This does not return topics that have no publishers. See getSystemState() to get more comprehensive list.
       * 
       * @param caller_id (str) ROS caller ID
       * @param subgraph (str) Restrict topic names to match within the specified subgraph. Subgraph namespace is resolved relative to the caller's namespace. Use emptry string to specify all names.
       * @returns Returns (int, str, [[str, str],]) (code, statusMessage, [ [topic1, type1]...[topicN, typeN] ])
       */
      virtual std::optional<TopicTypes> getPublishedTopics(const std::string& caller_id, const std::string& subgraph) { return std::nullopt; }

      /**
       * Subscribe the caller to the specified topic. In addition to receiving a list of current publishers, the subscriber will also receive notifications of new publishers via the publisherUpdate API.
       * 
       * @param caller_id (str) ROS caller ID
       * @param topic (str) Fully-qualified name of topic.
       * @param topic_type (str) Datatype for topic. Must be a package-resource name, i.e. the .msg name.
       * @param caller_api (str) API URI of subscriber to register. Will be used for new publisher notifications.
       * @returns Returns (int, str, [str]) (code, statusMessage, publishers)
       */
      virtual std::optional<PublishersInfo> registerSubscriber(const std::string& caller_id, const std::string& topicName, const std::string& topicType, const std::string& caller_api) { return std::nullopt; }

      /**
       * Unregister the caller as a publisher of the topic.
       * 
       * @param caller_id (str) ROS caller ID
       * @param topic (str) Fully-qualified name of topic.
       * @param caller_api (str) API URI of service to unregister. Unregistration will only occur if current registration matches.
       * @returns Returns (int, str, int) (code, statusMessage, numUnsubscribed) If numUnsubscribed is zero it means that the caller was not registered as a subscriber. The call still succeeds as the intended final state is reached.
       */
      virtual std::optional<UnregisterInfo> unregisterSubscriber(const std::string& caller_id, const std::string& topicName, const std::string& topicType, const std::string& caller_api) { return std::nullopt; }


      /**
       * Register the caller as a publisher the topic.
       * @param caller_id (str) ROS caller ID
       * @param topic (str) Fully-qualified name of topic to register.
       * @param topic_type (str) Datatype for topic. Must be a package-resource name, i.e. the .msg name.
       * @param caller_api (str) API URI of publisher to register.
       * @returns Returns (int, str, [str]) (code, statusMessage, subscriberApis) List of current subscribers of topic in the form of XMLRPC URIs.
       */
      virtual std::optional<SubscribersInfo> registerPublisher(const std::string& caller_id, const std::string& topicName, const std::string& topicType, const std::string& caller_api) { return std::nullopt; }

      /**
       * Unregister the caller as a publisher of the topic.
       * @param caller_id (str) ROS caller ID
       * @param topic (str) Fully-qualified name of topic to unregister.
       * @param caller_api (str) API URI of publisher to unregister. Unregistration will only occur if current registration matches.
       * @returns Returns (int, str, int) (code, statusMessage, numUnregistered) If numUnregistered is zero it means that the caller was not registered as a publisher. The call still succeeds as the intended final state is reached.
       */
      virtual std::optional<UnregisterInfo> unregisterPublisher(const std::string& caller_id, const std::string& topicName, const std::string& topicType, const std::string& caller_api) { return std::nullopt; }
      
      /**
       * Register the caller as a provider of the specified service.
       * @param caller_id (str) ROS caller ID
       * @param service (str) Fully-qualified name of service
       * @param service_api (str) ROSRPC Service URI
       * @param caller_api (str) XML-RPC URI of caller node
       */
      virtual std::optional<MasterMsg> registerService(const std::string& caller_id, const std::string& serviceName, const std::string& serviceUri, const std::string& caller_api) { return std::nullopt; }

      /**
       * Unregister the caller as a provider of the specified service.
       * 
       * @param caller_id (str) ROS caller ID
       * @param service (str) Fully-qualified name of service
       * @param service_api (str) API URI of service to unregister. Unregistration will only occur if current registration matches.
       * @returns Returns (int, str, int) (code, statusMessage, numUnregistered).
       */
      virtual std::optional<UnregisterInfo> unregisterService(const std::string& caller_id, const std::string& serviceName, const std::string& serviceUri, const std::string& caller_api) { return std::nullopt; }
    };
    
    /**
     * Creates Qualified ROSMaster API proxy object
     * 
     * This function can be manually set the ROSMaster URI. This means that this library is available multi master communication.
     * 
     * @param addr Host of ROSMaster (roscore)
     * @param port Port number of ROSMaster
     */
    NANOROS_API std::shared_ptr<ROSMaster> rosmaster(const std::string& addr, const int32_t port);

    /**
     * Creates Qualified ROSMaster API proxy object
     * 
     * This function automatically collect the ROS_MASTER_URI info from environmental variables
     */
    inline NANOROS_API std::shared_ptr<ROSMaster> rosmaster() {
      auto info = getROSMasterInfo();
      if (!info) { return std::make_shared<ROSMaster>(); }
      return rosmaster(info->first, info->second);
    }
  }
}
