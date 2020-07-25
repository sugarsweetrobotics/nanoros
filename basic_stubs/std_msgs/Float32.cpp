#include "nanoros/rosmsgstub.h"
#include "nanoros/rostcpros.h"
#include "Float32.h"

#include <cstdint>
#include <string>
#include <sstream>


namespace ssr::nanoros {
    namespace std_msgs {
        class Float32Stub : public ROSMsgStub {
        public:
            using DataType = Float32;
        public:
            Float32Stub() {}
            virtual ~Float32Stub() {}

        public:
            virtual std::string md5sum() const override { return "73fcbf46b49191e672908e50842a83d4"; }
            virtual std::string typeName() const override { return "std_msgs/Float32"; }
            virtual std::shared_ptr<const ROSMsg> toMsg(const std::optional<TCPROSPacket>& msg, int32_t& popedCount) const override { 
                //int32_t popedCount = 0;
                auto val = std::make_shared<DataType>();
                setValue(val, val->data, msg->pop<float>(popedCount));
                //auto v = msg->pop<float>(popedCount);
                //if (!v) {  return nullptr; }
                //return std::make_shared<DataType>(v.value());
                return val;
            }

            
            virtual std::shared_ptr<TCPROSPacket> toPacket(const ROSMsg& msg) const override {
                const auto val = static_cast<const DataType&>(msg);
                auto pkt = std::make_shared<TCPROSPacket>();
                pkt->push(val.data);
                return pkt; 
            }

            virtual std::shared_ptr<ROSMsg> fromJSON(const std::shared_ptr<const JSONObject> json) override { 
                if (!json) return nullptr;
                auto val = std::make_shared<DataType>();
                setValue(val->data, json);
                setValue<float>(val->data, json, "data");
                return val;
            }
        };
    }
}


void init_std_msgs_Int32(void* factory) {
    static_cast<ssr::nanoros::ROSMsgStubFactory*>(factory)->registerStub(std::make_shared<ssr::nanoros::std_msgs::Int32Stub>());
}
