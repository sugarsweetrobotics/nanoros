#pragma once
#include <memory>
#include <string>

#include "nanoros/rosmsgpacker.h"

namespace ssr {
  namespace nanoros {

    class ROSMsg;
    class ROSMsgPacker;
    class ROSNode;

    class ROSPublisher {
    protected:
      const std::string topicName_;
      ROSNode* node_;
    public:
      ROSPublisher(ROSNode* node, const std::string& topicName) : node_(node), topicName_(topicName) {}
      virtual ~ROSPublisher() {}

      virtual std::string getTopicName() const { return topicName_; }

      virtual std::string getTopicTypeName() const { return ""; }

      virtual bool isConnected() const { return false; }
    public:
      virtual bool publish(const std::shared_ptr<ROSMsg>& msg) {return false; }

      template<typename T>
      inline bool publish(const T& msg) {
        return publish(static_cast<const ROSMsg&>(msg));
      }
      virtual bool publish(const ROSMsg& msg) { return false; }
      virtual bool standBy(const std::string& caller_id, const std::string& selfIP, const int32_t port) { return false; }

    };


    std::shared_ptr<ROSPublisher> createROSPublisher(ROSNode* node, const std::string& topicName, const std::shared_ptr<ROSMsgPacker>& packer);

  }
}
