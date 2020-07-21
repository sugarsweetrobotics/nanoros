#pragma once

#include <string>

namespace ssr {
  namespace nanoros {

    class ROSMsg {
    public:
      ROSMsg() {}
      virtual ~ROSMsg() {}

    public:
      virtual std::string prettyString() const = 0;

    };
    
  }
}
