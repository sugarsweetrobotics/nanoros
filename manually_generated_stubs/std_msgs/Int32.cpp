#include "nanoros/nanoros_define.h"

#include "nanoros/rosmsgstub.h"
#include "nanoros/rostcpros.h"
#include "Int32.h"

#include <cstdint>
#include <string>
#include <sstream>


namespace ssr::nanoros {
    namespace std_msgs {


        class Int32Stub : public ROSMsgStub {
        private:
            using DataType = Int32;
        public:
            Int32Stub() {}
            virtual ~Int32Stub() {}

        public:
            virtual std::string md5sum() const override { return "da5909fbe378aeaf85e547e830cc1bb7"; }
            virtual std::string typeName() const override { return "std_msgs/Int32"; }
            virtual std::shared_ptr<const ROSMsg> toMsg(const std::optional<TCPROSPacket>& msg, int32_t& popedCount) override { 
                auto val = std::make_shared<DataType>();
                setValue(val, val->data, msg->pop<int32_t>(popedCount));
                return val;
            }

            
            virtual std::shared_ptr<TCPROSPacket> toPacket(const ROSMsg& msg) override {
                const auto val = dynamic_cast<const Int32&>(msg);
                auto pkt = std::make_shared<TCPROSPacket>();
                pkt->push(val.data);
                return pkt; 
            }

            virtual std::shared_ptr<ROSMsg> fromJSON(const std::shared_ptr<const JSONObject> json) override { 
                if (!json) return nullptr;
                auto val = std::make_shared<Int32>();
                setValue(val->data, json);
                setValue<int32_t>(val->data, json, "data");
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

    STUBFACTORY_EXPORT void init_msg_std_msgs_Int32(void* factory) {
        static_cast<ssr::nanoros::ROSMsgStubFactory*>(factory)->registerStub(std::make_shared<ssr::nanoros::std_msgs::Int32Stub>());
    }

}
