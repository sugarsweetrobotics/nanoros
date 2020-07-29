#pragma once

#include <string>
#include <sstream>

#include "nanoros/rosprimitive.h"

namespace ssr {
  namespace nanoros {



    //template<typename T>
    //std::string typeName() { return ""; }
    
    class ROSSrvResult {
    public:
      bool ok;
      std::string message;
    public:
      ROSSrvResult(): ok(true), message("") {}
      ROSSrvResult(const std::string& msg): ok(false), message(msg) {}
      virtual ~ROSSrvResult() {}

    public:
      virtual std::string prettyString(const std::string& indent="") const { return indent + "ok: " + (ok ? "true" : "false") + "¥n" + indent + "message: " + message + "¥n"; };

    };

    class ROSSrvArg {
    public:
      ROSSrvArg() {}
      virtual ~ROSSrvArg() {}

    public:
      virtual std::string prettyString(const std::string& indent="") const = 0;

    };


  }
}
