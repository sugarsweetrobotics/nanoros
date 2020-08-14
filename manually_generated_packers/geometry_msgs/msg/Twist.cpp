#include "nanoros/rosmsgpacker.h"
#include "nanoros/rostcpros.h"
#include "Twist.h"

#include <cstdint>
#include <string>
#include <sstream>


namespace ssr::nanoros {
    namespace geometry_msgs {



        class TwistPacker : public ROSMsgPacker {
        public:
            using DataType = Twist;
        public:
            TwistPacker() {}
            virtual ~TwistPacker() {}

        public:
            virtual std::string md5sum() const override { return "9f195f881246fdfa2798d1d3eebca84a"; }
            virtual std::string typeName() const override { return "geometry_msgs/Twist"; }

            virtual std::shared_ptr<const ROSMsg> toMsg(const std::optional<TCPROSPacket>& msg, int32_t& popedCount) override { 
                auto val = std::make_shared<DataType>();
                setValue(val->linear, getMsgPacker("geometry_msgs/Vector3"), msg, popedCount);
                setValue(val->angular, getMsgPacker("geometry_msgs/Vector3"), msg, popedCount);
                return val;
            }

            
            virtual std::shared_ptr<TCPROSPacket> toPacket(const ROSMsg& msg) override {
                const auto val = static_cast<const DataType&>(msg);
                auto pkt = std::make_shared<TCPROSPacket>();
                pushValue(pkt, getMsgPacker("geometry_msgs/Vector3"), val.linear);
                pushValue(pkt, getMsgPacker("geometry_msgs/Vector3"), val.angular);
                return pkt; 
            }

            virtual std::shared_ptr<ROSMsg> fromJSON(const std::shared_ptr<const JSONObject> json) override { 
                if (!json) return nullptr;
                auto val = std::make_shared<DataType>();
                setValue(val->linear, getMsgPacker("geometry_msgs/Vector3"), json, "linear");
                setValue(val->angular, getMsgPacker("geometry_msgs/Vector3"), json, "angular");
                return val;
            }
        };
    }
}


void init_msg_std_msgs_Twist(void* factory) {
    static_cast<ssr::nanoros::ROSMsgPackerFactory*>(factory)->registerPacker(std::make_shared<ssr::nanoros::geometry_msgs::TwistPacker>());
}
