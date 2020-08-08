
#pragma once
#include "nanoros/rosmsg.h"
#include "nanoros/rosmsgpackerfactory.h"
#include <sstream>
#include <string>

#include "std_msgs/msg/MultiArrayDimension.h"

namespace ssr::nanoros {

    namespace std_msgs {
        struct MultiArrayLayout : public ROSMsg {
            std::vector<std_msgs::MultiArrayDimension> dim; // # Array of dimension properties;
            uint32_t data_offset; // # padding elements at front of data;
            
            MultiArrayLayout(const std::vector<std_msgs::MultiArrayDimension>& _dim, const uint32_t& _data_offset):dim(_dim), data_offset(_data_offset){}
            MultiArrayLayout():dim(), data_offset(0){}
            virtual ~MultiArrayLayout(){}

            virtual std::string prettyString(const std::string& indent="") const {
                std::stringstream ss;
                ss << indent << "dim:" << std::endl;
                for(auto& elem : dim) {
                    ss << indent << " - " << std::endl;
                    ss << elem.prettyString(indent + "    ");
                }
                ss << indent << "data_offset" << ':' << data_offset << std::endl;
                return ss.str();
            }

            virtual std::string toJSON() const {
                std::stringstream ss;
                ss << '{';
                ss << "dim: " << std::endl;
                ss << '[';
                for(int i = 0;i < dim.size();i++) {
                    ss << dim[i].toJSON();
                    if (i != dim.size()-1) {
                        ss << ',';
                    }
                }
                ss << ']';
                ss << ',';
                ss << "data_offset" << ": " << data_offset << std::endl;
                ss << '}';
                return ss.str();
            }
        };
    }
    template<>
    std::string msgTypeName<std_msgs::MultiArrayLayout>() { return "std_msgs/MultiArrayLayout"; }
}
