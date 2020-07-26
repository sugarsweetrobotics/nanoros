#pragma once
#include <memory>
#include <string>

#include "nanoros/rosmsgstub.h"

namespace ssr {
  namespace nanoros {

    class ROSMsg;
    class ROSMsgStub;
    class ROSNode;

    class ROSPublisher {
    private:
      const std::string topicName_;
    public:
      ROSPublisher(const std::string& topicName) : topicName_(topicName) {}
      virtual ~ROSPublisher() {}

      virtual std::string topicName() const { return topicName_; }
    public:
      virtual bool publish(const std::shared_ptr<ROSMsg>& msg) {return false; }

      template<typename T>
      inline bool publish(const T& msg) {
        return publish(static_cast<const ROSMsg&>(msg));
      }
      virtual bool publish(const ROSMsg& msg) { return false; }
      virtual bool standBy(const std::string& caller_id, const std::string& selfIP, const int32_t port) { return false; }

    };


    std::shared_ptr<ROSPublisher> createROSPublisher(ROSNode* node, const std::string& topicName, const std::shared_ptr<ROSMsgStub>& stub);

  }
}
