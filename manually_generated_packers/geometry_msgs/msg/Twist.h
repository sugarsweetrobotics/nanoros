#pragma once
#include "nanoros/rosmsg.h"
#include "nanoros/rosmsgpackerfactory.h"
#include <sstream>
#include <string>

#include "geometry_msgs/msg/Vector3.h"

namespace ssr {
    namespace nanoros {
        namespace geometry_msgs {
            struct Twist : public ROSMsg {
                Vector3 linear;
                Vector3 angular;

                Twist() {}
                virtual ~Twist() {}

/*
                virtual std::string prettyString(const std::string& indent="") const {
                    std::stringstream ss;
                    ss << indent << "\"linear:" << std::endl;
                    ss << indent << linear.prettyString("  ");
                    ss << indent << "\"angular:" << std::endl;
                    ss << indent << angular.prettyString("  ");
                    return ss.str();
                }
*/
                virtual std::string toJSONString() const {
                    std::stringstream ss;
                    ss << '{';
                    ss << "\"linear\": " << linear.toJSON() << ',';
                    ss << "\"angular\": " << angular.toJSON();
                    ss << '}';
                    return ss.str();
                }
            };
        }


        template<>
        std::string msgTypeName<geometry_msgs::Twist>() { return "geometry_msgs/Twist"; }
    }
}

extern "C" {
void init_msg_std_msgs_Twist(void* factory);
}