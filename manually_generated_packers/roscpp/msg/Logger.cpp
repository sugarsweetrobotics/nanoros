#include "nanoros/rosmsgpacker.h"
#include "nanoros/rostcpros.h"
#include "Logger.h"

#include <cstdint>
#include <string>
#include <sstream>


namespace ssr::nanoros {
    namespace roscpp {
        class LoggerPacker : public ROSMsgPacker {

        public:
            using DataType = Logger;
        public:
            LoggerPacker() {}
            virtual ~LoggerPacker() {}

        public:
            virtual std::string md5sum() const override { return "a6069a2ff40db7bd32143dd66e1f408e"; }

            virtual std::string typeName() const override { return "roscpp/Logger"; }

            virtual std::shared_ptr<const ROSMsg> toMsg(const std::optional<TCPROSPacket>& msg, int32_t& popedCount) override { 
                auto val = std::make_shared<DataType>();
                setValue(val, val->name, msg->pop<std::string>(popedCount));
                setValue(val, val->level, msg->pop<std::string>(popedCount));
                return val;
            }
            
            virtual std::shared_ptr<TCPROSPacket> toPacket(const ROSMsg& msg) override {
                const auto val = static_cast<const DataType&>(msg);
                auto pkt = std::make_shared<TCPROSPacket>();
                pkt->push(val.name);
                pkt->push(val.level);
                return pkt; 
            }

            
            virtual std::shared_ptr<ROSMsg> fromJSON(const std::shared_ptr<const JSONObject> json) override { 
                if (!json) return nullptr;
                auto val = std::make_shared<DataType>();
                setValue<std::string>(val->name, json, "name");
                setValue<std::string>(val->level, json, "level");
                return val;
            }
        };
    }
}


void init_msg_roscpp_Logger(void* factory) {
    static_cast<ssr::nanoros::ROSMsgPackerFactory*>(factory)->registerPacker(std::make_shared<ssr::nanoros::roscpp::LoggerPacker>());
}
