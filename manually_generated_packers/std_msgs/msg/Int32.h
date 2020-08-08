#pragma once
#include "nanoros/rosmsg.h"
#include "nanoros/rosmsgpackerfactory.h"
#include <sstream>
#include <string>

namespace ssr::nanoros {
    namespace std_msgs {
        struct Int32 : public ROSMsg {
            int32_t data;
            Int32(const int32_t& arg0) : data(arg0) {}
            Int32(): Int32(0) {}
            virtual ~Int32() {}

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
    std::string msgTypeName<std_msgs::Int32>() { return "std_msgs/Int32"; }

}
