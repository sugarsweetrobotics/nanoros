#pragma once
#include "nanoros/rosmsg.h"
#include "nanoros/rosmsgpackerfactory.h"
#include <sstream>
#include <string>

namespace ssr {
    namespace nanoros {
        namespace std_msgs {
            struct Float32 : public ROSMsg {
                float data;

                Float32() : data(0) {}
                virtual ~Float32() {}
/*
                virtual std::string prettyString(const std::string& indent="") const {
                    std::stringstream ss;
                    ss << indent << "data" << ':' << data;
                    return ss.str();
                }
*/
                virtual std::string toJSONString() const {
                    std::stringstream ss;
                    ss << '{';
                    ss << "\"data\": " << data;
                    ss << '}';
                    return ss.str();
                }
            };
        }


        template<>
        std::string msgTypeName<std_msgs::Float32>() { return "std_msgs/Float32"; }
    }
}

extern "C" {
void init_msg_std_msgs_Float32(void* factory);
}