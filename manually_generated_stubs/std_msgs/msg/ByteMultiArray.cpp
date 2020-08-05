
#include "nanoros/nanoros_define.h"

#include "nanoros/rosmsgpacker.h"
#include "nanoros/rostcpros.h"
#include "ByteMultiArray.h"
namespace ssr::nanoros {
    namespace std_msgs {
        class ByteMultiArrayStub : public ROSMsgStub {
        private:
            using DataType = ByteMultiArray;
        public:
            ByteMultiArrayStub() {}
            virtual ~ByteMultiArrayStub() {}
        public:
            virtual std::string md5sum() const override { return "*"; }
            virtual std::string typeName() const override { return "std_msgs/ByteMultiArray"; }
            virtual std::shared_ptr<const ROSMsg> toMsg(const std::optional<TCPROSPacket>& msg, int32_t& popedCount) override {
                auto val = std::make_shared<DataType>();
                setValue(val->layout, getMsgStub("std_msgs/MultiArrayLayout"), msg, popedCount);
                pushValue(val->data, msg, popedCount);
                return val;
            }
            virtual std::shared_ptr<TCPROSPacket> toPacket(const ROSMsg& msg) override {
                const auto val = dynamic_cast<const DataType&>(msg);
                auto pkt = std::make_shared<TCPROSPacket>();
                pushValue(pkt, getMsgStub("std_msgs/MultiArrayLayout"), val.layout);
                pkt->pushVector(val.data);
                return pkt;
            }
            virtual std::shared_ptr<ROSMsg> fromJSON(const std::shared_ptr<const JSONObject> json) override {
                if (!json) return nullptr;
                auto val = std::make_shared<DataType>();
                setValue(val->layout, getMsgStub("std_msgs/MultiArrayLayout"), json, "layout");
                setArrayValue(val->data, json, "data");
                return val;
            }
        };
    }
}

#ifdef WIN32
#define STUBFACTORY_EXPORT __declspec(dllexport)
#else
#define STUBFACTORY_EXPORT
#endif

extern "C" {
    STUBFACTORY_EXPORT void init_msg_std_msgs_ByteMultiArray(void* factory) {
        static_cast<ssr::nanoros::ROSMsgStubFactory*>(factory)->registerStub(std::make_shared<ssr::nanoros::std_msgs::ByteMultiArrayStub>());
    }
}
