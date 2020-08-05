
#include "nanoros/nanoros_define.h"

#include "nanoros/rosmsgpacker.h"
#include "nanoros/rostcpros.h"
#include "MultiArrayLayout.h"
namespace ssr::nanoros {
    namespace std_msgs {
        class MultiArrayLayoutStub : public ROSMsgStub {
        private:
            using DataType = MultiArrayLayout;
        public:
            MultiArrayLayoutStub() {}
            virtual ~MultiArrayLayoutStub() {}
        public:
            virtual std::string md5sum() const override { return "*"; }
            virtual std::string typeName() const override { return "std_msgs/MultiArrayLayout"; }
            virtual std::shared_ptr<const ROSMsg> toMsg(const std::optional<TCPROSPacket>& msg, int32_t& popedCount) override {
                auto val = std::make_shared<DataType>();
                pushValue(val->dim, getMsgStub("std_msgs/MultiArrayDimension"), msg, popedCount);
                setValue(val, val->data_offset, msg->pop<uint32_t>(popedCount));
                return val;
            }
            virtual std::shared_ptr<TCPROSPacket> toPacket(const ROSMsg& msg) override {
                const auto val = dynamic_cast<const DataType&>(msg);
                auto pkt = std::make_shared<TCPROSPacket>();
                pushVectorValue(pkt, getMsgStub("std_msgs/MultiArrayDimension"), val.dim);
                pkt->push(val.data_offset);
                return pkt;
            }
            virtual std::shared_ptr<ROSMsg> fromJSON(const std::shared_ptr<const JSONObject> json) override {
                if (!json) return nullptr;
                auto val = std::make_shared<DataType>();
                setArrayValue(val->dim, getMsgStub("std_msgs/MultiArrayDimension"), json, "dim");
                setValue<uint32_t>(val->data_offset, json, "data_offset");
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
    STUBFACTORY_EXPORT void init_msg_std_msgs_MultiArrayLayout(void* factory) {
        static_cast<ssr::nanoros::ROSMsgStubFactory*>(factory)->registerStub(std::make_shared<ssr::nanoros::std_msgs::MultiArrayLayoutStub>());
    }
}
