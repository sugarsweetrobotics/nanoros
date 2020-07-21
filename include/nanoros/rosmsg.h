#pragma once

#include <string>
#include <sstream>

namespace ssr {
  namespace nanoros {

    class ROSMsg {
    public:
      ROSMsg() {}
      virtual ~ROSMsg() {}

    public:
      virtual std::string prettyString(const std::string& indent="") const = 0;

    };

    struct time {
      uint32_t sec;
      uint32_t nsec;

      time(): sec(0), nsec(0) {}
      time(uint32_t s, uint32_t ns) : sec(s), nsec(ns) {}

      virtual std::string prettyString(const std::string& indent="") const {
        std::stringstream ss;
        ss << indent << "sec:" << sec << std::endl;
        ss << indent << "nsec:" << nsec;
        return ss.str();
      }
    };
    
  }
}
