#pragma once
#include "nanoros/rosmsg.h"
#include "nanoros/rosmsgstubfactory.h"
#include <sstream>
#include <string>

namespace ssr {
    namespace nanoros {


        namespace std_msgs {
            struct String : public ROSMsg {
                std::string data;
                String(const std::string& d): data(d) {}
                String(): String("") {}
                virtual ~String() {}

                virtual std::string prettyString(const std::string& indent="") const {
                    std::stringstream ss;
                    ss << indent << "data" << ':' << data;
                    return ss.str();
                }
            };
        }


        template<>
        std::string typeName<std_msgs::String>() { return "std_msgs/String"; }
    }
}

extern "C" {
void init_msg_std_msgs_String(void* factory);
}