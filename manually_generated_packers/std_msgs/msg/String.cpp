#include "nanoros/rosmsgpacker.h"
#include "nanoros/rostcpros.h"
#include "String.h"

#include <cstdint>
#include <string>
#include <sstream>


namespace ssr::nanoros {
    namespace std_msgs {


        class StringPacker : public ROSMsgPacker {
        private:
            using DataType = String;
        public:
            StringPacker() {}
            virtual ~StringPacker() {}

        public:
            virtual std::string md5sum() const override { return "992ce8a1687cec8c8bd883ec73ca41d1"; }
            virtual std::string typeName() const override { return "std_msgs/String"; }
            virtual std::shared_ptr<const ROSMsg> toMsg(const std::optional<TCPROSPacket>& msg, int32_t& popedCount) override { 
                auto val = std::make_shared<DataType>();
                setValue(val, val->data, msg->pop<std::string>(popedCount));
                return val;
            }

            
            virtual std::shared_ptr<TCPROSPacket> toPacket(const ROSMsg& msg) override {
                const auto val = dynamic_cast<const String&>(msg);
                auto pkt = std::make_shared<TCPROSPacket>();
                pkt->push(val.data);
                return pkt; 
            }

            virtual std::shared_ptr<ROSMsg> fromJSON(const std::shared_ptr<const JSONObject> json) override { 
                if (!json) return nullptr;
                auto val = std::make_shared<String>();
                setValue(val->data, json);
                setValue<std::string>(val->data, json, "data");
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
    PACKERFACTORY_EXPORT void init_msg_std_msgs_String(void* factory) {
        static_cast<ssr::nanoros::ROSMsgPackerFactory*>(factory)->registerPacker(std::make_shared<ssr::nanoros::std_msgs::StringPacker>());
    }
}
