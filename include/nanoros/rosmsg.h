#pragma once

#include <string>
#include <sstream>

#include "nanoros/nanoros.h"
#include "nanoros/json.h"
#include "nanoros/rosprimitive.h"

namespace ssr {
  namespace nanoros {



    template<typename T>
    std::string msgTypeName() { return ""; }
    
    class NANOROS_API ROSMsg {
    public:
      ROSMsg() {}
      virtual ~ROSMsg() {}

    public:
      virtual std::string prettyString(const std::string& indent="") const;
      virtual std::string toJSONString() const = 0;
      virtual std::shared_ptr<const JSONObject> toJSON() const {
        return fromJSONString(toJSONString());
      }
    };

  }
}
