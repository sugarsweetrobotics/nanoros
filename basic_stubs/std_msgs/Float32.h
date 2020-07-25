#pragma once
#include "nanoros/rosmsg.h"
#include "nanoros/rosmsgstubfactory.h"
#include <sstream>
#include <string>

namespace ssr {
    namespace nanoros {
        namespace std_msgs {
            struct Float32 : public ROSMsg {
                float data;

                Float32(const float& arg0) : data(arg0) {}
                virtual ~Float32() {}

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
void init_std_msgs_Float32(void* factory);
}