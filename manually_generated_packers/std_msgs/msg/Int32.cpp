#include "nanoros/nanoros_define.h"

#include "nanoros/rosmsgpacker.h"
#include "nanoros/rostcpros.h"
#include "Int32.h"

#include <cstdint>
#include <string>
#include <sstream>


namespace ssr::nanoros {
    namespace std_msgs {


        class Int32Packer : public ROSMsgPacker {
        private:
            using DataType = Int32;
        public:
            Int32Packer() {}
            virtual ~Int32Packer() {}

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
#define PACKERFACTORY_EXPORT __declspec(dllexport)
#else
#define PACKERFACTORY_EXPORT
#endif

extern "C" {

    PACKERFACTORY_EXPORT void init_msg_std_msgs_Int32(void* factory) {
        static_cast<ssr::nanoros::ROSMsgPackerFactory*>(factory)->registerPacker(std::make_shared<ssr::nanoros::std_msgs::Int32Packer>());
    }

}
