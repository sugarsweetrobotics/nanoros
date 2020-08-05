#pragma once

#include <memory>
#include <string>

#include "nanoros/packerfactory.h"
#include "nanoros/nanoros_define.h"
#include "nanoros/rosmsgpacker.h"

namespace ssr {
  namespace nanoros {
    class ROSMsgStub;
    
    class ROSMsgStubFactory : public StubFactory<ROSMsgStub> {
    public:
      ROSMsgStubFactory() {}
      virtual ~ROSMsgStubFactory() {}

    };

    NANOROS_API std::shared_ptr<ROSMsgStubFactory> getROSMsgStubFactory();
  }
}
