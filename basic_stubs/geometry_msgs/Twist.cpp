#include "nanoros/rosmsgstub.h"
#include "nanoros/rostcpros.h"
#include "Twist.h"

#include <cstdint>
#include <string>
#include <sstream>


namespace ssr::nanoros {
    namespace geometry_msgs {



        class TwistStub : public ROSMsgStub {
        public:
            using DataType = Twist;
        public:
            TwistStub() {}
            virtual ~TwistStub() {}

        public:
            virtual std::string md5sum() const override { return "9f195f881246fdfa2798d1d3eebca84a"; }
            virtual std::string typeName() const override { return "geometry_msgs/Twist"; }

            virtual std::shared_ptr<const ROSMsg> toMsg(const std::optional<TCPROSPacket>& msg, int32_t& popedCount) const override { 
                auto val = std::make_shared<DataType>();
                setValue(val->linear, getStub("Vector3"), msg, popedCount);
                setValue(val->angular, getStub("Vector3"), msg, popedCount);
                return val;
            }

            
            virtual std::shared_ptr<TCPROSPacket> toPacket(const ROSMsg& msg) const override {
                const auto val = static_cast<const DataType&>(msg);
                auto pkt = std::make_shared<TCPROSPacket>();
                pushValue(pkt, getStub("Vector3"), val.linear);
                pushValue(pkt, getStub("Vector3"), val.angular);
                return pkt; 
            }

            virtual std::shared_ptr<ROSMsg> fromJSON(const std::shared_ptr<const JSONObject> json) override { 
                if (!json) return nullptr;
                auto val = std::make_shared<DataType>();
                setValue(val->linear, getStub("Vector3"), json, "linear");
                setValue(val->angular, getStub("Vector3"), json, "angular");
                return val;
            }
        };
    }
}


void init_std_msgs_Int32(void* factory) {
    static_cast<ssr::nanoros::ROSMsgStubFactory*>(factory)->registerStub(std::make_shared<ssr::nanoros::geometry_msgs::TwistStub>());
}
