

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
#include "GetLoggers.h"

namespace ssr::nanoros {
    namespace roscpp {


        class GetLoggersStub : public ROSSrvStub {
            using RequestType = GetLoggersRequest;
            using ResponseType = GetLoggersResponse;
        public:
            GetLoggersStub() {}
            virtual ~GetLoggersStub() {}

        public:
            virtual std::string md5sum() const override { return "32e97e85527d4678a8f9279894bb64b0"; }

            virtual std::string typeName() const override { return "roscpp/GetLoggers"; }

            virtual std::shared_ptr<const ROSSrvResponse> toSrvResponse(const std::optional<TCPROSPacket>& msg, int32_t& popedCount) override { 
                auto val = std::make_shared<ResponseType>();
                pushValue(val->loggers, getMsgStub("roscpp/Logger"), msg, popedCount);
                return val;
            }

            virtual std::shared_ptr<const ROSSrvRequest> toSrvRequest(const std::optional<TCPROSPacket>& msg, int32_t& popedCount) override { 
                auto val = std::make_shared<RequestType>();
                return val;
            }

            virtual std::shared_ptr<TCPROSPacket> toPacket(const ROSSrvRequest& msg) override {
                const auto val = static_cast<const RequestType&>(msg);
                auto pkt = std::make_shared<TCPROSPacket>();
                return pkt; 
            }

            virtual std::shared_ptr<TCPROSPacket> toPacket(const ROSSrvResponse& msg) override {
                const auto val = static_cast<const ResponseType&>(msg);
                auto pkt = std::make_shared<TCPROSPacket>();
                for(auto logger : val.loggers) {
                    pushValue(pkt, getMsgStub("roscpp/Logger"), logger);
                }
                return pkt; 
            }

            virtual std::shared_ptr<ROSSrvRequest> fromJSON(const std::shared_ptr<const JSONObject> json) override { 
                if (!json) return nullptr;
                auto val = std::make_shared<RequestType>();
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