#pragma once

#include "nanoros/nanoros_define.h"
#include <memory>
#include <string>

#include "nanoros/packerfactory.h"
#include "nanoros/rossrvpacker.h"

namespace ssr {
  namespace nanoros {

    class ROSSrvPackerFactory : public PackerFactory<ROSSrvPacker> {
    public:
      ROSSrvPackerFactory() {}
      virtual ~ROSSrvPackerFactory() {}

    public:

    };

    NANOROS_API std::shared_ptr<ROSSrvPackerFactory> getROSSrvPackerFactory();
  }
}
