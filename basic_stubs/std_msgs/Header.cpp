#include "nanoros/rosmsgstub.h"
#include "nanoros/rostcpros.h"
#include "Header.h"

#include <cstdint>
#include <string>
#include <sstream>


namespace ssr::nanoros {
    namespace std_msgs {
        class HeaderStub : public ROSMsgStub {
        public:
            HeaderStub() {}
            virtual ~HeaderStub() {}

        public:
            virtual std::string md5sum() const override { return "2176decaecbce78abc3b96ef049fabed"; }

            virtual std::string typeName() const override { return "std_msgs/Header"; }

            virtual std::shared_ptr<const ROSMsg> toMsg(const std::optional<TCPROSPacket>& msg) const override { 
                int32_t popedCount = 0;
                auto arg0 = msg->pop<uint32_t>(popedCount);
                if (!arg0) {  return nullptr; }
                auto arg1 = msg->pop<ssr::nanoros::time>(popedCount);
                if (!arg1) {  return nullptr; }
                auto arg2 = msg->pop<std::string>(popedCount);
                if (!arg2) {  return nullptr; }
                return std::make_shared<Header>(arg0.value(), arg1.value(), arg2.value());
            }
            
            virtual std::shared_ptr<TCPROSPacket> toPacket(const std::shared_ptr<ROSMsg>& msg) const override {
                const auto val = std::static_pointer_cast<const Header>(msg);
                auto pkt = std::make_shared<TCPROSPacket>();
                pkt->push(val->seq);
                pkt->push(val->stamp);
                pkt->push(val->frame_id);
                return pkt; 
            }

            virtual std::shared_ptr<ROSMsg> fromString(const std::string& str) override { 
                //int32_t val = atoi(str.c_str());
                //return std::make_shared<Header>(val); 
            }
        };
    }
}


void init_std_msgs_Header(void* factory) {
    static_cast<ssr::nanoros::ROSMsgStubFactory*>(factory)->registerStub(std::make_shared<ssr::nanoros::std_msgs::HeaderStub>());
}
