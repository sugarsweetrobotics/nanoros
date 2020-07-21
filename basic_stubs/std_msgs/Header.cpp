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
            virtual std::string md5sum() const { return "2176decaecbce78abc3b96ef049fabed"; }
            virtual std::string typeName() const { return "std_msgs/Header"; }
            virtual std::shared_ptr<const ROSMsg> toMsg(const std::optional<TCPROSPacket>& msg) const { 
                int32_t popedCount = 0;
                auto arg0 = msg->popInt32(popedCount);
                if (!arg0) {  return nullptr; }
                auto arg1 = msg->popTime(popedCount);
                if (!arg1) {  return nullptr; }
                auto arg2 = msg->popString(popedCount);
                if (!arg2) {  return nullptr; }
                return std::make_shared<Header>(arg0.value(), arg1.value(), arg2.value());
            }
        };
    }
}


void init_std_msgs_Header(void* factory) {
    static_cast<ssr::nanoros::ROSMsgStubFactory*>(factory)->registerStub(std::make_shared<ssr::nanoros::std_msgs::HeaderStub>());
}
