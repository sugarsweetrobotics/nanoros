
#pragma once
#include "nanoros/rosmsg.h"
#include "nanoros/rosmsgpackerfactory.h"
#include <sstream>
#include <string>

#include "std_msgs/msg/MultiArrayLayout.h"

namespace ssr::nanoros {

    namespace std_msgs {
        struct ByteMultiArray : public ROSMsg {
            std_msgs::MultiArrayLayout layout; // # specification of data layout;
            std::vector<uint8_t> data; // # array of data;
            
            ByteMultiArray(const std_msgs::MultiArrayLayout& _layout, const std::vector<uint8_t>& _data):layout(_layout), data(_data){}
            ByteMultiArray():layout(), data(){}
            virtual ~ByteMultiArray(){}
/*
            virtual std::string prettyString(const std::string& indent="") const {
                std::stringstream ss;
                ss << indent << "layout:" << std::endl;
                ss << layout.prettyString(indent + "    ");
                ss << indent << "data" << ": [";
                auto it = data.begin();
                while (true) {
                    ss << *it;
                    ++it;
                    if (it == data.end()) break;
                    ss << ',';
                }
                ss << ']' << std::endl;
                return ss.str();
            }
*/
            virtual std::string toJSONString() const {
                std::stringstream ss;
                ss << '{';
                ss << "\"layout\": " << layout.toJSON() << ',';
                ss << "\"data\": ";
                ss << '[';
                for(int i = 0;i < data.size();i++) {
                    ss << data[i];
                    if (i != data.size() -1) {
                        ss << ',';
                    }
                }
                ss << ']';
                ss << '}';
                return ss.str();
            }
        };
    }
    template<>
    std::string msgTypeName<std_msgs::ByteMultiArray>() { return "std_msgs/ByteMultiArray"; }
}
