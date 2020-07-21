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

                Int32(int32_t d) : data(d) {}
                virtual ~Int32() {}

                virtual std::string prettyString() const {
                    std::stringstream ss;
                    ss << "data" << ':' << data << std::endl;
                    return ss.str();
                }
            };
        }
    }
}

extern "C" {
void init_std_msgs_Int32(void* factory);
}