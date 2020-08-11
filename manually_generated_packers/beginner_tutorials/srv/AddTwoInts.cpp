

#include "nanoros/rossrvpacker.h"
#include "nanoros/rostcpros.h"
#include "nanoros/rossrv.h"
#include "nanoros/rossrvpackerfactory.h"
#include "nanoros/rosmsgpackerfactory.h"

#include "AddTwoInts.h"

#include <cstdint>
#include <string>
#include <sstream>

#include <sstream>
#include <string>
#include <utility>
#include <optional>
#include <memory>


namespace ssr::nanoros {
    namespace beginner_tutorials {

        class AddTwoIntsRequestPacker : public ROSMsgPacker {
        public:
            using DataType = AddTwoIntsRequest;

        public:
            virtual std::string md5sum() const override { return "73fcbf46b49191e672908e50842a83d4"; }
            virtual std::string typeName() const override { return "beginner_tutorials/AddTwoIntsRequest"; }
            virtual std::shared_ptr<const ROSMsg> toMsg(const std::optional<TCPROSPacket>& msg, int32_t& popedCount) override { 
                auto val = std::make_shared<DataType>();
                setValue(val, val->a, msg->pop<int64_t>(popedCount));
                setValue(val, val->b, msg->pop<int64_t>(popedCount));
                return val;
            }

            
            virtual std::shared_ptr<TCPROSPacket> toPacket(const ROSMsg& msg) override {
                const auto val = static_cast<const DataType&>(msg);
                auto pkt = std::make_shared<TCPROSPacket>();
                pkt->push(val.a);
                pkt->push(val.b);
                return pkt; 
            }

            virtual std::shared_ptr<ROSMsg> fromJSON(const std::shared_ptr<const JSONObject> json) override { 
                if (!json) return nullptr;
                auto val = std::make_shared<DataType>();
                setValue<int64_t>(val->a, json, "a");
                setValue<int64_t>(val->b, json, "b");
                return val;
            }
        };

        class AddTwoIntsResponsePacker : public ROSMsgPacker {
 public:
            using DataType = AddTwoIntsResponse;

        public:
            virtual std::string md5sum() const override { return "73fcbf46b49191e672908e50842a83d4"; }
            virtual std::string typeName() const override { return "beginner_tutorials/AddTwoIntsResponse"; }
                        virtual std::shared_ptr<const ROSMsg> toMsg(const std::optional<TCPROSPacket>& msg, int32_t& popedCount) override { 
                auto val = std::make_shared<DataType>();
                setValue(val, val->sum, msg->pop<int64_t>(popedCount));
                return val;
            }

            
            virtual std::shared_ptr<TCPROSPacket> toPacket(const ROSMsg& msg) override {
                const auto val = static_cast<const DataType&>(msg);
                auto pkt = std::make_shared<TCPROSPacket>();
                pkt->push(val.sum);
                return pkt; 
            }

            virtual std::shared_ptr<ROSMsg> fromJSON(const std::shared_ptr<const JSONObject> json) override { 
                if (!json) return nullptr;
                auto val = std::make_shared<DataType>();
                setValue<int64_t>(val->sum, json, "sum");
                return val;
            }
        };

        class AddTwoIntsPacker : public AbstractROSSrvPacker<AddTwoIntsRequestPacker, AddTwoIntsResponsePacker>  {
        public:

        public:
            virtual std::string md5sum() const override { return "6a2e34150c00229791cc89ff309fff21"; }

            virtual std::string typeName() const override { return "beginner_tutorials/AddTwoInts"; }

            
        };
    }
}

extern "C" {
    
    void init_srv_beginner_tutorials_AddTwoInts(void* factory) {
        static_cast<ssr::nanoros::ROSSrvPackerFactory*>(factory)->registerPacker(std::make_shared<ssr::nanoros::beginner_tutorials::AddTwoIntsPacker>());


       // ssr::nanoros::getROSMsgPackerFactory()->registerPacker(std::make_shared<ssr::nanoros::beginner_tutorials::AddTwoIntsRequestPacker>());
       // ssr::nanoros::getROSMsgPackerFactory()->registerPacker(std::make_shared<ssr::nanoros::beginner_tutorials::AddTwoIntsResponsePacker>());
    }
}