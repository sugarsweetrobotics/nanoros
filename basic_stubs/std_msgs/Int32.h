#pragma once
#include "nanoros/rosmsg.h"
#include "nanoros/rosmsgstubfactory.h"
#include <sstream>
#include <string>

namespace ssr {
    namespace nanoros {
        namespace std_msgs {
            struct Int32 : public ROSMsg {
                int32_t data;

                Int32(const int32_t& arg0) : data(arg0) {}
                virtual ~Int32() {}

                virtual std::string prettyString(const std::string& indent="") const {
                    std::stringstream ss;
                    ss << indent << "data" << ':' << data;
                    return ss.str();
                }
            };
        }
    }
}

extern "C" {
void init_std_msgs_Int32(void* factory);
}