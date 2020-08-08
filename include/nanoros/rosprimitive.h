#pragma once

#include <cstdint>
#include <string>
#include <sstream>

namespace ssr {

    namespace nanoros {


        using float64 = double;
        using float32 = float;

        struct time {
            uint32_t sec;
            uint32_t nsec;

            time(): sec(0), nsec(0) {}
            time(uint32_t s, uint32_t ns) : sec(s), nsec(ns) {}

            virtual std::string prettyString(const std::string& indent="") const {
                std::stringstream ss;
                ss << indent << "sec:" << sec << std::endl;
                ss << indent << "nsec:" << nsec;
                return ss.str();
            }
            
            virtual std::string toJSON() const {
                std::stringstream ss;
                ss << '{';
                ss << "sec: " << sec << ',';
                ss << "nsec: " << nsec;
                ss << '}';
                return ss.str();
            }
        };

    }
}