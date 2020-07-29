#pragma once

#include <string>
#include <sstream>

#include "nanoros/rosprimitive.h"

namespace ssr {
  namespace nanoros {



    template<typename T>
    std::string typeName() { return ""; }
    
    class ROSMsg {
    public:
      ROSMsg() {}
      virtual ~ROSMsg() {}

    public:
      virtual std::string prettyString(const std::string& indent="") const = 0;

    };

  }
}
