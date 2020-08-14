#pragma once

#include <string>
#include <sstream>

#include "nanoros/rosprimitive.h"

#include "nanoros/rosmsg.h"

namespace ssr {
  namespace nanoros {

    /*
    class ROSSrvResponse : public ROSMsg{
    public:
      bool ok;
      std::string message;
    public:
      ROSSrvResponse(): ok(true), message("") {}
      ROSSrvResponse(const std::string& msg): ok(false), message(msg) {}
      virtual ~ROSSrvResponse() {}

    public:
      // virtual std::string prettyString(const std::string& indent="") const { return indent + "ok: " + (ok ? "true" : "false") + "¥n" + indent + "message: " + message + "¥n"; };
      virtual std::string toJSONString() const { 
        return "";
      }
    };

    class ROSSrvRequest : public ROSMsg {
    public:
      ROSSrvRequest() {}
      virtual ~ROSSrvRequest() {}

    public:
      // virtual std::string prettyString(const std::string& indent="") const = 0;
      virtual std::string toJSONString() const { 
        return "";
      }
    };
    */

    template<typename T>
    std::string srvTypeName() { return ""; }

    template<typename T>
    std::string srvResponseTypeName() { return ""; }

    template<typename T>
    std::string srvRequestTypeName() { return ""; }

  }
}
