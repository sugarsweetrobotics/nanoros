#pragma once
#include "nanoros/rosmsg.h"
#include "nanoros/rosmsgstubfactory.h"
#include <sstream>
#include <string>
#include <optional>
#include <memory>

namespace ssr {
    namespace nanoros {
        namespace roscpp {
            struct Logger : public ROSMsg {
                std::string name;
                std::string level;

                Logger(const std::string& arg0, const std::string& arg1) : name(arg0), level(arg1) {}
                Logger(): name(""), level("") {}
                virtual ~Logger() {}

                virtual std::string prettyString(const std::string& indent="") const {
                    std::stringstream ss;
                    ss << indent << "name: \"" << name << "\"" << std::endl;
                    ss << indent << "level: \"" << level << "\"" << std::endl;
                    return ss.str();
                }
            };
        }
    }
}

extern "C" {
void init_msg_roscpp_Logger(void* factory);
}