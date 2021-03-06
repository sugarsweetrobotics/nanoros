#pragma once

#include <memory>
#include <functional>
#include <string>

#include "nanoros/rosmsgpacker.h"

#include "nanoros/rosslave.h"


namespace ssr {
  namespace nanoros {
    class ROSNode;
    class ROSMsg;


    class ROSSubscriber {
    protected:
      ROSNode* node_;
      const std::string topicName_;

    public:
      virtual std::string getTopicName() const { return topicName_; }
      virtual std::string getTopicTypeName() const { return ""; }
    public:
      ROSSubscriber(ROSNode* node, const std::string& topicName) : node_(node), topicName_(topicName) {}
      virtual ~ROSSubscriber() {}
    public:
      
      virtual bool connect(const std::string& uri, const bool latching=true, const double negotiateTimeout=1.0) { return false;}
      virtual bool disconnect() { return false; }
      virtual bool disconnectUri(const std::string& uri) { return false; }
      virtual void spinOnce();

      virtual std::optional<std::vector<std::string>> getSubscribingPublisherUris() { return std::nullopt; }
    };


    std::shared_ptr<ROSSubscriber> createROSSubscriber(ROSNode* node, const std::string& topicName, const std::shared_ptr<ROSMsgPacker>& packer, const std::function<void(const std::shared_ptr<const ROSMsg>& msg)>& func);
  }
}
