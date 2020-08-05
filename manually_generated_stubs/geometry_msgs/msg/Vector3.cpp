#include "nanoros/rosmsgpacker.h"
#include "nanoros/rostcpros.h"
#include "Vector3.h"

#include <cstdint>
#include <string>
#include <sstream>


namespace ssr::nanoros {
    namespace geometry_msgs {
        class Vector3Stub : public ROSMsgStub {
        public:
            using DataType = Vector3;
        public:
            Vector3Stub() {}
            virtual ~Vector3Stub() {}

        public:
            virtual std::string md5sum() const override { return "4a842b65f413084dc2b10fb484ea7f17"; }
            virtual std::string typeName() const override { return "geometry_msgs/Vector3"; }
            virtual std::shared_ptr<const ROSMsg> toMsg(const std::optional<TCPROSPacket>& msg, int32_t& popedCount) override { 
//                int32_t popedCount = 0;
                auto val = std::make_shared<DataType>();
                setValue(val, val->x, msg->pop<float64>(popedCount));
                setValue(val, val->y, msg->pop<float64>(popedCount));
                setValue(val, val->z, msg->pop<float64>(popedCount));
                return val;
            }

            
            virtual std::shared_ptr<TCPROSPacket> toPacket(const ROSMsg& msg) override {
                const auto val = dynamic_cast<const DataType&>(msg);
                auto pkt = std::make_shared<TCPROSPacket>();
                pkt->push(val.x);
                pkt->push(val.y);
                pkt->push(val.z);
                return pkt; 
            }

            virtual std::shared_ptr<ROSMsg> fromJSON(const std::shared_ptr<const JSONObject> json) override { 
                if (!json) return nullptr;
                auto val = std::make_shared<DataType>();
                setValue<float64>(val->x, json, "x");
                setValue<float64>(val->y, json, "y");
                setValue<float64>(val->z, json, "z");
                return val;
            }
        };
    }
}


void init_msg_std_msgs_Vector3(void* factory) {
    static_cast<ssr::nanoros::ROSMsgStubFactory*>(factory)->registerStub(std::make_shared<ssr::nanoros::geometry_msgs::Vector3Stub>());
}
