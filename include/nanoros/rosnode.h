#pragma once
#include <memory>
#include <string>
#include <functional>
#include "nanoros/rosmaster.h"
#include "nanoros/rosmsgstub.h"

namespace ssr {
  namespace nanoros {
    class ROSSubscriber;
    class ROSSlaveServer;

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
    public:
      virtual std::shared_ptr<ROSSubscriber> subscribe(const std::string& topicName, const std::shared_ptr<ROSMsgStub>& stub, 
      const std::function<void(const std::shared_ptr<const ROSMsg>& msg)>& func, const bool latching=false, const double negotiateTimeout=1.0) {return nullptr; }
    };

    
    std::shared_ptr<ROSNode> registerROSNode(const std::shared_ptr<ROSMaster>& master, const std::string& nodeName);
    inline std::shared_ptr<ROSNode> registerROSNode(const std::string& nodeName) {
      return registerROSNode(rosmaster(), nodeName);
    }

  }
}
