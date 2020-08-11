#pragma once





#include "nanoros/rossrv.h"


#include <sstream>
#include <string>
#include <optional>
#include <memory>


#include <cstdint>
#include <string>
#include <sstream>

namespace ssr::nanoros {
    namespace beginner_tutorials {

        class AddTwoIntsRequest : public ROSMsg {
        public:
            int64_t a;
            int64_t b;

            virtual std::string toJSONString() const {
                std::stringstream ss;
                ss << "{";
                ss << "\"a\": " << a << ",";
                ss << "\"b\": " << b;
                ss << "}";
                return ss.str();
            }

        };


        class AddTwoIntsResponse : public ROSMsg{
        public:
            int64_t sum;
        public:
            virtual std::string toJSONString() const {
                std::stringstream ss;
                ss << "{";
                ss << "\"sum\": " << sum;
                ss << "}";
                return ss.str();
            }
        };
    }



    template<>
    std::string srvResponseTypeName<beginner_tutorials::AddTwoIntsResponse>() { return "beginner_tutorials/AddTwoIntsRequest"; }

    template<>
    std::string srvTypeName<beginner_tutorials::AddTwoIntsResponse>() { return "beginner_tutorials/AddTwoInts"; }


    template<>
    std::string srvRequestTypeName<beginner_tutorials::AddTwoIntsRequest>() { return "beginner_tutorials/AddTwoIntsRequest"; }

    template<>
    std::string srvTypeName<beginner_tutorials::AddTwoIntsRequest>() { return "beginner_tutorials/AddTwoInts"; }
}