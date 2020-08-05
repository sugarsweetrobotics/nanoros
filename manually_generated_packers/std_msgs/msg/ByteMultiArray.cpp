
#include "nanoros/nanoros_define.h"

#include "nanoros/rosmsgpacker.h"
#include "nanoros/rostcpros.h"
#include "ByteMultiArray.h"
namespace ssr::nanoros {
    namespace std_msgs {
        class ByteMultiArrayPacker : public ROSMsgPacker {
        private:
            using DataType = ByteMultiArray;
        public:
            ByteMultiArrayPacker() {}
            virtual ~ByteMultiArrayPacker() {}
        public:
            virtual std::string md5sum() const override { return "*"; }
            virtual std::string typeName() const override { return "std_msgs/ByteMultiArray"; }
            virtual std::shared_ptr<const ROSMsg> toMsg(const std::optional<TCPROSPacket>& msg, int32_t& popedCount) override {
                auto val = std::make_shared<DataType>();
                setValue(val->layout, getMsgPacker("std_msgs/MultiArrayLayout"), msg, popedCount);
                pushValue(val->data, msg, popedCount);
                return val;
            }
            virtual std::shared_ptr<TCPROSPacket> toPacket(const ROSMsg& msg) override {
                const auto val = dynamic_cast<const DataType&>(msg);
                auto pkt = std::make_shared<TCPROSPacket>();
                pushValue(pkt, getMsgPacker("std_msgs/MultiArrayLayout"), val.layout);
                pkt->pushVector(val.data);
                return pkt;
            }
            virtual std::shared_ptr<ROSMsg> fromJSON(const std::shared_ptr<const JSONObject> json) override {
                if (!json) return nullptr;
                auto val = std::make_shared<DataType>();
                setValue(val->layout, getMsgPacker("std_msgs/MultiArrayLayout"), json, "layout");
                setArrayValue(val->data, json, "data");
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
    PACKERFACTORY_EXPORT void init_msg_std_msgs_ByteMultiArray(void* factory) {
        static_cast<ssr::nanoros::ROSMsgPackerFactory*>(factory)->registerPacker(std::make_shared<ssr::nanoros::std_msgs::ByteMultiArrayPacker>());
    }
}
