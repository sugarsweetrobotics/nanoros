#pragma once
#include "nanoros/rosmsg.h"
#include "nanoros/rosmsgstubfactory.h"
#include <sstream>
#include <string>

namespace ssr {
    namespace nanoros {
        namespace geometry_msgs {
            struct Vector3 : public ROSMsg {
                float64 x;
                float64 y;
                float64 z;

                Vector3() {}
                virtual ~Vector3() {}

                virtual std::string prettyString(const std::string& indent="") const {
                    std::stringstream ss;
                    ss << indent << "x: " << x << std::endl;
                    ss << indent << "y: " << y << std::endl;
                    ss << indent << "z: " << z << std::endl;
                    return ss.str();
                }
            };
        }


        template<>
        std::string msgTypeName<geometry_msgs::Vector3>() { return "geometry_msgs/Vector3"; }
    }
}

extern "C" {
void init_msg_std_msgs_Vector3(void* factory);
}