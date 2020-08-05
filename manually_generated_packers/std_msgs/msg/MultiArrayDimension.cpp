
#include "nanoros/nanoros_define.h"

#include "nanoros/rosmsgpacker.h"
#include "nanoros/rostcpros.h"
#include "MultiArrayDimension.h"
namespace ssr::nanoros {
    namespace std_msgs {
        class MultiArrayDimensionPacker : public ROSMsgPacker {
        private:
            using DataType = MultiArrayDimension;
        public:
            MultiArrayDimensionPacker() {}
            virtual ~MultiArrayDimensionPacker() {}
        public:
            virtual std::string md5sum() const override { return "*"; }
            virtual std::string typeName() const override { return "std_msgs/MultiArrayDimension"; }
            virtual std::shared_ptr<const ROSMsg> toMsg(const std::optional<TCPROSPacket>& msg, int32_t& popedCount) override {
                auto val = std::make_shared<DataType>();
                setValue(val, val->label, msg->pop<std::string>(popedCount));
                setValue(val, val->size, msg->pop<uint32_t>(popedCount));
                setValue(val, val->stride, msg->pop<uint32_t>(popedCount));
                return val;
            }
            virtual std::shared_ptr<TCPROSPacket> toPacket(const ROSMsg& msg) override {
                const auto val = dynamic_cast<const DataType&>(msg);
                auto pkt = std::make_shared<TCPROSPacket>();
                pkt->push(val.label);
                pkt->push(val.size);
                pkt->push(val.stride);
                return pkt;
            }
            virtual std::shared_ptr<ROSMsg> fromJSON(const std::shared_ptr<const JSONObject> json) override {
                if (!json) return nullptr;
                auto val = std::make_shared<DataType>();
                setValue<std::string>(val->label, json, "label");
                setValue<uint32_t>(val->size, json, "size");
                setValue<uint32_t>(val->stride, json, "stride");
                return val;
            }
        };
    }
}

#ifdef WIN32
#define PACKERFACTORY_EXPORT __declspec(dllexport)
#else
#define PACKERFACTORY_EXPORT
#endif

extern "C" {
    PACKERFACTORY_EXPORT void init_msg_std_msgs_MultiArrayDimension(void* factory) {
        static_cast<ssr::nanoros::ROSMsgPackerFactory*>(factory)->registerPacker(std::make_shared<ssr::nanoros::std_msgs::MultiArrayDimensionPacker>());
    }
}
