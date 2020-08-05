

#include "nanoros/rossrvpacker.h"
#include "nanoros/rostcpros.h"
#include "nanoros/rossrv.h"
#include "nanoros/rossrvpackerfactory.h"

#include "AddTwoInts.h"

#include <cstdint>
#include <string>
#include <sstream>

#include <sstream>
#include <string>
#include <utility>
#include <optional>
#include <memory>


namespace ssr::nanoros {
    namespace beginner_tutorials {


        class AddTwoIntsPacker : public ROSSrvPacker {
        public:
            using RequestType = AddTwoIntsRequest;
            using ResponseType = AddTwoIntsResponse;
        public:
            AddTwoIntsPacker() {}
            virtual ~AddTwoIntsPacker() {}

        public:
            virtual std::string md5sum() const override { return "6a2e34150c00229791cc89ff309fff21"; }

            virtual std::string typeName() const override { return "beginner_tutorials/AddTwoInts"; }

            virtual std::shared_ptr<const ROSSrvResponse> toSrvResponse(const std::optional<TCPROSPacket>& msg, int32_t& popedCount) override { 
                auto val = std::make_shared<ResponseType>();
                setValue(val, val->sum, msg->pop<int64_t>(popedCount));
                return val;
            }

            virtual std::shared_ptr<const ROSSrvRequest> toSrvRequest(const std::optional<TCPROSPacket>& msg, int32_t& popedCount) override { 
                auto val = std::make_shared<RequestType>();
                setValue(val, val->a, msg->pop<int64_t>(popedCount));
                setValue(val, val->b, msg->pop<int64_t>(popedCount));
                return val;
            }
            
            virtual std::shared_ptr<TCPROSPacket> toPacket(const ROSSrvRequest& msg) override {
                const auto val = static_cast<const RequestType&>(msg);
                auto pkt = std::make_shared<TCPROSPacket>();
                pkt->push(val.a);
                pkt->push(val.b);
                return pkt; 
            }

            virtual std::shared_ptr<TCPROSPacket> toPacket(const ROSSrvResponse& msg) override {
                const auto val = static_cast<const ResponseType&>(msg);
                auto pkt = std::make_shared<TCPROSPacket>();
                pkt->push(val.sum);
                return pkt; 
            }
            
            virtual std::shared_ptr<ROSSrvRequest> fromJSON(const std::shared_ptr<const JSONObject> json) override { 
                if (!json) return nullptr;
                auto val = std::make_shared<RequestType>();
                setValue<int64_t>(val->a, json, "a");
                setValue<int64_t>(val->b, json, "b");
                return val;
            }
        };
    }
}

extern "C" {
    
    void init_srv_beginner_tutorials_AddTwoInts(void* factory) {
        static_cast<ssr::nanoros::ROSSrvPackerFactory*>(factory)->registerPacker(std::make_shared<ssr::nanoros::beginner_tutorials::AddTwoIntsPacker>());
    }
}