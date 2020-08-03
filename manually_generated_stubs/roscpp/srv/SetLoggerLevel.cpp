

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

#include "SetLoggerLevel.h"
#include "roscpp/msg/Logger.h"

namespace ssr::nanoros {
    namespace roscpp {

        class SetLoggerLevelStub : public ROSSrvStub {
        public:
            using RequestType = SetLoggerLevelRequest;
            using ResponseType = SetLoggerLevelResponse;
        public:
            SetLoggerLevelStub() {}
            virtual ~SetLoggerLevelStub() {}

        public:
            virtual std::string md5sum() const override { return "51da076440d78ca1684d36c868df61ea"; }

            virtual std::string typeName() const override { return "roscpp/SetLoggerLevel"; }

            virtual std::shared_ptr<const ROSSrvResponse> toSrvResponse(const std::optional<TCPROSPacket>& msg, int32_t& popedCount) override { 
                auto val = std::make_shared<ResponseType>();
                return val;
            }

            virtual std::shared_ptr<const ROSSrvRequest> toSrvRequest(const std::optional<TCPROSPacket>& msg, int32_t& popedCount) override { 
                auto val = std::make_shared<RequestType>();
                setValue(val, val->logger, msg->pop<std::string>(popedCount));
                setValue(val, val->level, msg->pop<std::string>(popedCount));
                return val;
            }

            virtual std::shared_ptr<TCPROSPacket> toPacket(const ROSSrvRequest& msg) override {
                const auto val = static_cast<const RequestType&>(msg);
                auto pkt = std::make_shared<TCPROSPacket>();
                pkt->push(val.logger);
                pkt->push(val.level);
                return pkt; 
            }

            virtual std::shared_ptr<TCPROSPacket> toPacket(const ROSSrvResponse& msg) override {
                const auto val = static_cast<const ResponseType&>(msg);
                auto pkt = std::make_shared<TCPROSPacket>();
                return pkt; 
            }
            
            virtual std::shared_ptr<ROSSrvRequest> fromJSON(const std::shared_ptr<const JSONObject> json) override { 
                if (!json) return nullptr;
                auto val = std::make_shared<RequestType>();
                setValue<std::string>(val->logger, json, "logger");
                setValue<std::string>(val->level, json, "level");
                return val;
            }
        };
    }
}

extern "C" {
    void init_srv_roscpp_SetLoggerLevel(void* factory) {
        static_cast<ssr::nanoros::ROSSrvStubFactory*>(factory)->registerStub(std::make_shared<ssr::nanoros::roscpp::SetLoggerLevelStub>());
    }
}