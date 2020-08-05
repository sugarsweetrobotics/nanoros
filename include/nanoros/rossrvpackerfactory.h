#pragma once

#include "nanoros/nanoros_define.h"
#include <memory>
#include <string>

#include "nanoros/packerfactory.h"
#include "nanoros/rossrvpacker.h"

namespace ssr {
  namespace nanoros {

    class ROSSrvStubFactory : public StubFactory<ROSSrvStub> {
    public:
      ROSSrvStubFactory() {}
      virtual ~ROSSrvStubFactory() {}

    public:

    };

    NANOROS_API std::shared_ptr<ROSSrvStubFactory> getROSSrvStubFactory();
  }
}
