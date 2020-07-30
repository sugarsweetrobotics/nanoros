#pragma once
#include <memory>
#include <string>
#include <functional>
#include "nanoros/nanoros_define.h"

#include "nanoros/rosmaster.h"
#include "nanoros/rosmsgstub.h"
#include "nanoros/rosmsgstubfactory.h"
#include "nanoros/rossrvstub.h"
#include "nanoros/rossrvstubfactory.h"
#include "nanoros/rospublisher.h"
#include "nanoros/rosrate.h"
#include "nanoros/rosserviceprovider.h"

namespace ssr {
  namespace nanoros {
    class ROSSubscriber;
    class ROSSlaveServer;
    class ROSPublisher;

    class ROSNode {
    protected:
      const std::string name_;
    public:
      ROSNode(const std::string& name) : name_(name) {}
      virtual ~ROSNode() {}
    public:
      virtual std::pair<int32_t, int32_t> getPortRange() const { return {30000, 65535}; }

      virtual std::shared_ptr<ROSSlaveServer> slaveServer() { return nullptr; }
      virtual std::string name() const { return name_; }
      virtual void spinOnce() {}
      virtual void spin() {}
      virtual void spin(const Rate& rate) {}

      virtual std::shared_ptr<ROSPublisher> getRegisteredPublisher(const std::string& topicName) const { return nullptr; }

      virtual std::shared_ptr<ROSSubscriber> getRegisteredSubscriber(const std::string& topicName) const { return nullptr; }


      virtual std::shared_ptr<ROSServiceProvider> getRegisteredServiceProvider(const std::string& srvName) const { return nullptr; }
    public:
      virtual std::shared_ptr<ROSPublisher> advertise(const std::string& topicName, const std::shared_ptr<ROSMsgStub>& stub, const double negotiateTimeout=1.0) {return nullptr; }

      template<typename T>
      std::shared_ptr<ROSPublisher> advertise(const std::string& topicName, const double negotiateTimeout=1.0) {
        auto factory = getROSMsgStubFactory();
        if (!factory) return nullptr;
        auto stub = factory->getStub(msgTypeName<T>());
        if (!stub) return nullptr;
        return advertise(topicName, stub, negotiateTimeout);
      }

      virtual bool unadvertise(const std::string& topicName) {
        return false;
      }

      virtual bool unadvertise(const std::shared_ptr<ROSPublisher> & pub) {
        return false;
      }

  
      virtual void unadvertiseAll() {
        
      }

      virtual std::shared_ptr<ROSSubscriber> subscribe(const std::string& topicName, const std::shared_ptr<ROSMsgStub>& stub, 
          const std::function<void(const std::shared_ptr<const ROSMsg>& msg)>& func, const bool latching=false, const double negotiateTimeout=1.0) {return nullptr; }

      template<typename T>
      std::shared_ptr<ROSSubscriber> subscribe(const std::string& topicName, const std::function<void(const std::shared_ptr<const T>& msg)>& func, const bool latching=false, const double negotiateTimeout=1.0) {
        auto factory = getROSMsgStubFactory();
        if (!factory) return nullptr;
        auto stub = factory->getStub(msgTypeName<T>());
        if (!stub) return nullptr;
        return subscribe(topicName, stub, [func](const std::shared_ptr<const ROSMsg>& msg) {
          func(std::dynamic_pointer_cast<const T>(msg));
        }, latching, negotiateTimeout);
      }

      virtual bool unsubscribe(const std::string& topicName) { return false; }


      virtual bool subscribeUri(const std::string& topicName, const std::string& uri, const bool latching=false, const double negotiateTimeout=1.0) { return false; }

      virtual bool unsubscribeUri(const std::string& topicName, const std::string& uri) { return false; }


      virtual void unsubscribeAll() { return; }


      virtual void unadvertiseServiceAll() { return; }

      template<typename ReqType, typename ResType>
      bool advertiseService(const std::string& srvName, const std::function<const std::shared_ptr<ResType>(const std::shared_ptr<const ReqType>&)>& func) {
        auto factory = getROSSrvStubFactory();
        if (!factory) return false;
        auto stub = factory->getStub(srvTypeName<ReqType>());
        if (!stub) return false;
        return advertiseService(srvName, stub, [func, stub](const std::shared_ptr<const ROSSrvRequest>& request) -> const std::shared_ptr<ROSSrvResponse> {
          const std::shared_ptr<const ReqType> req = std::dynamic_pointer_cast<const ReqType>(request);
          //const std::shared_ptr<ResType> res = std::dynamic_pointer_cast<ResType>(response);
          if(!req) return nullptr;
          return func(req);
        });
      }

      virtual bool advertiseService(const std::string& srvName, const std::shared_ptr<ROSSrvStub>& stub, const std::function<const std::shared_ptr<ROSSrvResponse>(const std::shared_ptr<const ROSSrvRequest>&)>& func) {
        return false; 
      }

      virtual bool unadvertiseService(const std::shared_ptr<ROSServiceProvider> & provider) {
        return false;
      }

      virtual std::optional<std::vector<TopicTypeInfo>> getPublications() const { return std::nullopt; }

      virtual std::optional<std::vector<TopicTypeInfo>> getSubscriptions() const { return std::nullopt; }

      virtual std::optional<std::vector<std::string>> getCurrentSubscribingPublisherUris() const { return std::nullopt; }

    };

    
    NANOROS_API std::shared_ptr<ROSNode> registerROSNode(const std::shared_ptr<ROSMaster>& master, const std::string& nodeName);
    
    inline std::shared_ptr<ROSNode> registerROSNode(const std::string& nodeName) {
      return registerROSNode(rosmaster(), nodeName);
    }

  }
}
