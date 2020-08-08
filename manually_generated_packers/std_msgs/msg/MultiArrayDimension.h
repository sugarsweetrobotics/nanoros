
#pragma once
#include "nanoros/rosmsg.h"
#include "nanoros/rosmsgpackerfactory.h"
#include <sstream>
#include <string>


namespace ssr::nanoros {

    namespace std_msgs {
        struct MultiArrayDimension : public ROSMsg {
            std::string label; // # label of given dimension;
            uint32_t size; // # size of given dimension (in type units);
            uint32_t stride; // # stride of given dimension;
            
            MultiArrayDimension(const std::string& _label, const uint32_t& _size, const uint32_t& _stride):label(_label), size(_size), stride(_stride){}
            MultiArrayDimension():label(""), size(0), stride(0){}
            virtual ~MultiArrayDimension(){}

/*
            virtual std::string prettyString(const std::string& indent="") const {
                std::stringstream ss;
                ss << indent << "label" << ':' << label << std::endl;
                ss << indent << "size" << ':' << size << std::endl;
                ss << indent << "stride" << ':' << stride << std::endl;
                return ss.str();
            }
*/
            virtual std::string toJSONString() const {
                std::stringstream ss;
                ss << '{';
                ss << "\"label\": " << label << ',';
                ss << "\"size\": " << size << ',';
                ss << "\"stride\": " << stride;
                ss << '}'; 
                return ss.str();
            }
        };
    }
    template<>
    std::string msgTypeName<std_msgs::MultiArrayDimension>() { return "std_msgs/MultiArrayDimension"; }
}
