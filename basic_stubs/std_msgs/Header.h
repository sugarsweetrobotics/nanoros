#pragma once
#include "nanoros/rosmsg.h"
#include "nanoros/rosmsgstubfactory.h"
#include <sstream>
#include <string>

namespace ssr {
    namespace nanoros {
        namespace std_msgs {
            struct Header : public ROSMsg {
                uint32_t seq;
                ssr::nanoros::time stamp;
                std::string frame_id;

                Header(const uint32_t& arg0, const ssr::nanoros::time& arg1, const std::string& arg2) : seq(arg0), stamp(arg1), frame_id(arg2) {}
                virtual ~Header() {}

                virtual std::string prettyString(const std::string& indent="") const {
                    std::stringstream ss;
                    ss << indent << "seq:" << seq << std::endl;
                    ss << indent << "stamp:\n" << stamp.prettyString(indent + "  ") << std::endl;
                    ss << indent << "frame_id:" << '\'' << frame_id << '\'';
                    return ss.str();
                }
            };
        }
    }
}

extern "C" {
void init_std_msgs_Header(void* factory);
}