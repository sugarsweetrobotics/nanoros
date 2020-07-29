

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

        class SetLoggerLevelArg : public ROSSrvArg {
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


        class SetLoggerLevelResult: public ROSSrvResult {
        public:
            virtual std::string prettyString(const std::string& indent="") const {
                std::stringstream ss;
                return ss.str();
            }
        };

        class SetLoggerLevelStub : public ROSSrvStub {
        public:
            using ResultType = SetLoggerLevelResult;
            using ArgType = SetLoggerLevelArg;
        public:
            SetLoggerLevelStub() {}
            virtual ~SetLoggerLevelStub() {}

        public:
            virtual std::string md5sum() const override { return "51da076440d78ca1684d36c868df61ea"; }

            virtual std::string typeName() const override { return "roscpp/SetLoggerLevel"; }

            virtual std::shared_ptr<const ROSSrvResult> toSrvResult(const std::optional<TCPROSPacket>& msg, int32_t& popedCount) override { 
                auto val = std::make_shared<ResultType>();
                return val;
            }
            
            virtual std::shared_ptr<TCPROSPacket> toPacket(const ROSSrvArg& msg) override {
                const auto val = static_cast<const ArgType&>(msg);
                auto pkt = std::make_shared<TCPROSPacket>();
                pkt->push(val.logger);
                pkt->push(val.level);
                return pkt; 
            }

            
            virtual std::shared_ptr<ROSSrvArg> fromJSON(const std::shared_ptr<const JSONObject> json) override { 
                if (!json) return nullptr;
                auto val = std::make_shared<ArgType>();
                setValue<std::string>(val->logger, json, "logger");
                setValue<std::string>(val->level, json, "level");
                return val;
            }
        };
    }
}

extern "C" {
    void init_srv_roscpp_SetLoggerLevelStub(void* factory) {
        static_cast<ssr::nanoros::ROSSrvStubFactory*>(factory)->registerStub(std::make_shared<ssr::nanoros::roscpp::SetLoggerLevelStub>());
    }
}