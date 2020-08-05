#pragma once



#include "nanoros/rossrvpacker.h"
#include "nanoros/rostcpros.h"
#include "nanoros/rossrv.h"
#include "nanoros/rossrvpackerfactory.h"
#include <sstream>
#include <string>
#include <optional>
#include <memory>


#include <cstdint>
#include <string>
#include <sstream>

#include "roscpp/msg/Logger.h"

namespace ssr::nanoros {
    namespace roscpp {

        class GetLoggersRequest : public ROSSrvRequest {
        public:
            virtual std::string prettyString(const std::string& indent="") const {
                std::stringstream ss;
                ss << "\n";
                return ss.str();
            }
        };


        class GetLoggersResponse : public ROSSrvResponse {
        public:
            std::vector<Logger> loggers;


            virtual std::string prettyString(const std::string& indent="") const {
                std::stringstream ss;
                ss << indent << "loggers:" << std::endl;
                for(auto& logger: loggers) {
                    ss << indent + "  " + "-" << std::endl;
                    ss << logger.prettyString(indent + "    ");
                }
                return ss.str();
            }
        };

    }


    template<>
    std::string srvResponseTypeName<roscpp::GetLoggersResponse>() { return "roscpp/GetLoggersResponse"; }

    template<>
    std::string srvTypeName<roscpp::GetLoggersResponse>() { return "roscpp/GetLoggers"; }


    template<>
    std::string srvRequestTypeName<roscpp::GetLoggersRequest>() { return "roscpp/GetLoggersRequest"; }

    template<>
    std::string srvTypeName<roscpp::GetLoggersRequest>() { return "roscpp/GetLoggers"; }
}