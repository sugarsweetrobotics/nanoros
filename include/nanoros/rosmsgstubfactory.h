#pragma once

#include <memory>
#include <string>

#include "nanoros/rosmsgstub.h"

namespace ssr {
  namespace nanoros {
    class ROSMsgStub;
    
    class ROSMsgStubFactory {
    public:
      ROSMsgStubFactory() {}
      virtual ~ROSMsgStubFactory() {}

    public:
      virtual void registerStub(const std::shared_ptr<ROSMsgStub>& stub) = 0;
      virtual std::shared_ptr<ROSMsgStub> getStub(const std::string& topicTypeName) = 0;
    };

    std::shared_ptr<ROSMsgStubFactory> getROSMsgStubFactory();
  }
}
