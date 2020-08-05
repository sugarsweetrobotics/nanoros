#pragma once

#include <memory>
#include <string>

#include "nanoros/packerfactory.h"
#include "nanoros/nanoros_define.h"
#include "nanoros/rosmsgpacker.h"

namespace ssr {
  namespace nanoros {
    class ROSMsgPacker;
    
    class ROSMsgPackerFactory : public PackerFactory<ROSMsgPacker> {
    public:
      ROSMsgPackerFactory() {}
      virtual ~ROSMsgPackerFactory() {}

    };

    NANOROS_API std::shared_ptr<ROSMsgPackerFactory> getROSMsgPackerFactory();
  }
}
