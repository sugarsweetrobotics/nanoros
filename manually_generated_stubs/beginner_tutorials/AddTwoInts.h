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

        class AddTwoIntsRequest : public ROSSrvRequest {
        public:
            int64_t a;
            int64_t b;

            virtual std::string prettyString(const std::string& indent="") const {
                std::stringstream ss;
                ss << indent << "a: " << a << std::endl;
                ss << indent << "b: " << b << std::endl;
                return ss.str();
            }

        };


        class AddTwoIntsResponse : public ROSSrvResponse {
        public:
            int64_t sum;
        public:
            virtual std::string prettyString(const std::string& indent="") const {
                std::stringstream ss;
                ss << indent << "sum: " << sum << std::endl;
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