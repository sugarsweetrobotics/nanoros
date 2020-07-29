#pragma once

#include <memory>
#include <string>

#include "nanoros/rossrvstub.h"

namespace ssr {
  namespace nanoros {

    class ROSSrvStubFactory {
    public:
      ROSSrvStubFactory() {}
      virtual ~ROSSrvStubFactory() {}

    public:
      virtual void registerStub(const std::shared_ptr<ROSSrvStub>& stub) = 0;
      virtual std::shared_ptr<ROSSrvStub> getStub(const std::string& topicTypeName) = 0;
    };

    std::shared_ptr<ROSSrvStubFactory> getROSSrvStubFactory();
  }
}
