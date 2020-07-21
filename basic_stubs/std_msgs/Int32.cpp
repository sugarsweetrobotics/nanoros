#include "nanoros/rosmsgstub.h"
#include "nanoros/rostcpros.h"
#include "Int32.h"

#include <cstdint>
#include <string>
#include <sstream>


namespace ssr::nanoros {
    namespace std_msgs {
        class Int32Stub : public ROSMsgStub {
        public:
            Int32Stub() {}
            virtual ~Int32Stub() {}

        public:
            virtual std::string md5sum() const { return "da5909fbe378aeaf85e547e830cc1bb7"; }
            virtual std::string typeName() const { return "std_msgs/Int32"; }
            virtual std::shared_ptr<const ROSMsg> toMsg(const std::optional<TCPROSPacket>& msg) const { 
                int32_t popedCount = 0;
                auto v = msg->popInt32(popedCount);
                if (!v) {  return nullptr; }
                return std::make_shared<Int32>(v.value());
            }
        };
    }
}


void init_std_msgs_Int32(void* factory) {
    static_cast<ssr::nanoros::ROSMsgStubFactory*>(factory)->registerStub(std::make_shared<ssr::nanoros::std_msgs::Int32Stub>());
}
