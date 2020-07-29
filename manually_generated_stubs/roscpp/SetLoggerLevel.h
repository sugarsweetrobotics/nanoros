

#include "nanoros/rossrvstub.h"
#include "nanoros/rostcpros.h"
#include "nanoros/rossrv.h"
#include "nanoros/rossrvstubfactory.h"
#include <sstream>
#include <string>
#include <optional>
#include <memory>


#include <cstdint>
#include <string>
#include <sstream>

#include "roscpp/Logger.h"

namespace ssr::nanoros {
    namespace roscpp {

        class SetLoggerLevelRequest : public ROSSrvRequest {
        public:
            std::string logger;
            std::string level;

            virtual std::string prettyString(const std::string& indent="") const {
                std::stringstream ss;
                ss << indent << "logger:" << logger << std::endl;
                ss << indent << "level:" << level << std::endl;
                return ss.str();
            }

        };


        class SetLoggerLevelResponse : public ROSSrvResponse {
        public:
            virtual std::string prettyString(const std::string& indent="") const {
                std::stringstream ss;
                return ss.str();
            }
        };

    }

    template<>
    std::string srvResponseTypeName<roscpp::SetLoggerLevelResponse>() { return "roscpp/SetLoggerLevelResponse"; }

    template<>
    std::string srvTypeName<roscpp::SetLoggerLevelResponse>() { return "roscpp/SetLoggerLevel"; }


    template<>
    std::string srvRequestTypeName<roscpp::SetLoggerLevelRequest>() { return "roscpp/SetLoggerLevelRequest"; }

    template<>
    std::string srvTypeName<roscpp::SetLoggerLevelRequest>() { return "roscpp/SetLoggerLevel"; }
}