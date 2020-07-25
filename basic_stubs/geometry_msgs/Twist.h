#pragma once
#include "nanoros/rosmsg.h"
#include "nanoros/rosmsgstubfactory.h"
#include <sstream>
#include <string>

#include "geometry_msgs/Vector3.h"

namespace ssr {
    namespace nanoros {
        namespace geometry_msgs {
            struct Twist : public ROSMsg {
                Vector3 linear;
                Vector3 angular;

                Twist() {}
                virtual ~Twist() {}

                virtual std::string prettyString(const std::string& indent="") const {
                    std::stringstream ss;
                    ss << indent << "linear:" << std::endl;
                    ss << indent << linear.prettyString("  ");
                    ss << indent << "angular:" << std::endl;
                    ss << indent << angular.prettyString("  ");
                    return ss.str();
                }
            };
        }
    }
}

extern "C" {
void init_std_msgs_Float32(void* factory);
}