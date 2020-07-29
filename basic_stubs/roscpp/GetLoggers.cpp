

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

        class GetLoggersArg : public ROSSrvArg {
        public:
            virtual std::string prettyString(const std::string& indent="") const {
                std::stringstream ss;
                ss << "\n";
                return ss.str();
            }
        };


        class GetLoggersResult: public ROSSrvResult {
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

        class GetLoggersStub : public ROSSrvStub {
            using ResultType = GetLoggersResult;
            using ArgType = GetLoggersArg;
        public:
            GetLoggersStub() {}
            virtual ~GetLoggersStub() {}

        public:
            virtual std::string md5sum() const override { return "32e97e85527d4678a8f9279894bb64b0"; }

            virtual std::string typeName() const override { return "roscpp/GetLoggers"; }

            virtual std::shared_ptr<const ROSSrvResult> toSrvResult(const std::optional<TCPROSPacket>& msg, int32_t& popedCount) override { 
                auto val = std::make_shared<ResultType>();
                /*
                auto stub = getMsgStub("roscpp/Logger");
                if (!stub) return nullptr;
                auto size = msg->pop<uint32_t>(popedCount);
                if (!size) return nullptr;
                for(int i = 0;i < size;i++) {
                    auto v = std::dynamic_pointer_cast<const Logger>(stub->toMsg(msg, popedCount));
                    if (!v) return nullptr;
                    val->loggers.push_back(*v.get());
                }
                return val;
                */
                pushValue(val->loggers, getMsgStub("roscpp/Logger"), msg, popedCount);
                return val;
            }
            
            virtual std::shared_ptr<TCPROSPacket> toPacket(const ROSSrvArg& msg) override {
                const auto val = static_cast<const ArgType&>(msg);
                auto pkt = std::make_shared<TCPROSPacket>();
                return pkt; 
            }

            
            virtual std::shared_ptr<ROSSrvArg> fromJSON(const std::shared_ptr<const JSONObject> json) override { 
                if (!json) return nullptr;
                auto val = std::make_shared<ArgType>();
                return val;
            }
        };
    }
}

extern "C" {
    void init_srv_roscpp_GetLoggers(void* factory) {
        static_cast<ssr::nanoros::ROSSrvStubFactory*>(factory)->registerStub(std::make_shared<ssr::nanoros::roscpp::GetLoggersStub>());
    }
}