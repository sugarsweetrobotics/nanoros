#include "nanoros/rosmsgpacker.h"
#include "nanoros/rostcpros.h"
#include "Header.h"

#include <cstdint>
#include <string>
#include <sstream>


namespace ssr::nanoros {
    namespace std_msgs {
        class HeaderPacker : public ROSMsgPacker {

        public:
            using DataType = Header;
        public:
            HeaderPacker() {}
            virtual ~HeaderPacker() {}

        public:
            virtual std::string md5sum() const override { return "2176decaecbce78abc3b96ef049fabed"; }

            virtual std::string typeName() const override { return "std_msgs/Header"; }

            virtual std::shared_ptr<const ROSMsg> toMsg(const std::optional<TCPROSPacket>& msg, int32_t& popedCount) override { 
                auto val = std::make_shared<DataType>();
                setValue(val, val->seq, msg->pop<uint32_t>(popedCount));
                setValue(val, val->stamp, msg->pop<ssr::nanoros::time>(popedCount));
                setValue(val, val->frame_id, msg->pop<std::string>(popedCount));
                return val;
            }
            
            virtual std::shared_ptr<TCPROSPacket> toPacket(const ROSMsg& msg) override {
                const auto val = dynamic_cast<const Header&>(msg);
                auto pkt = std::make_shared<TCPROSPacket>();
                pkt->push(val.seq);
                pkt->push(val.stamp);
                pkt->push(val.frame_id);
                return pkt; 
            }

            
            virtual std::shared_ptr<ROSMsg> fromJSON(const std::shared_ptr<const JSONObject> json) override { 
                if (!json) return nullptr;
                auto val = std::make_shared<Header>();
                setValue<uint32_t>(val->seq, json, "seq");
                setValue<ssr::nanoros::time>(val->stamp, json, "stamp");
                setValue<std::string>(val->frame_id, json, "frame_id");
                return val;
            }
        };
    }
}


void init_msg_std_msgs_Header(void* factory) {
    static_cast<ssr::nanoros::ROSMsgPackerFactory*>(factory)->registerPacker(std::make_shared<ssr::nanoros::std_msgs::HeaderPacker>());
}
