#pragma once

#include "nanoros/nanoros_define.h"
#include "nanoros/rossrv.h"
#include "nanoros/json.h"
#include "nanoros/rostcpros.h"
#include "nanoros/rostcprospacket.h"
#include "nanoros/rosmsgpacker.h"

#include <string>
#include <map>
#include <optional>

namespace ssr {
  namespace nanoros {


    class TCPROSPacket;

    class NANOROS_API ROSSrvPacker {
    protected:

    public:
      ROSSrvPacker() {}
      virtual ~ROSSrvPacker() {}

    public:
      virtual std::string md5sum() const { return ""; }
      virtual std::string typeName() const { return ""; }

      virtual std::shared_ptr<const ROSMsg> toSrvRequest(const std::optional<TCPROSPacket>& msg) { 
        int32_t popedCount = 0;
        return toSrvRequest(msg, popedCount);
      }

      virtual std::shared_ptr<const ROSMsg> toSrvResponse(const std::optional<TCPROSPacket>& msg) { 
        int32_t popedCount = 0;
        return toSrvResponse(msg, popedCount);
      }

      virtual std::shared_ptr<const ROSMsg> toSrvResponse(const std::optional<TCPROSPacket>& msg, int32_t& popedCount) {
        return nullptr;
      }

      virtual std::shared_ptr<const ROSMsg> toSrvRequest(const std::optional<TCPROSPacket>& msg, int32_t& popedCount) { 
        return nullptr;
      }

      virtual std::shared_ptr<TCPROSPacket> requestToPacket(const ROSMsg& msg) {
        return nullptr;
      }
      virtual std::shared_ptr<TCPROSPacket> responseToPacket(const ROSMsg& msg) {
        return nullptr;
      }

      virtual std::shared_ptr<ROSMsg> fromJSON(const std::shared_ptr<const JSONObject>& json) = 0;

    };


    template<typename ReqPkr, typename ResPkr>
    class AbstractROSSrvPacker : public ROSSrvPacker {
    protected:
      std::map<std::string, std::shared_ptr<ROSMsgPacker>> packers_;

      ReqPkr requestPacker;

      ResPkr responsePacker;

    public:
      std::shared_ptr<ROSMsgPacker> getMsgPacker(const std::string& key) {
          if (packers_.count(key) > 0) return packers_.at(key);

          auto factory = getROSMsgPackerFactory();
          auto packer = factory->getPacker(key);
          if (!packer) return nullptr;
          packers_[key] = packer;
          return packer;
      }

    public:
      AbstractROSSrvPacker() {}
      virtual ~AbstractROSSrvPacker() {}

    public:
      virtual std::string md5sum() const { return ""; }
      virtual std::string typeName() const { return ""; }

      virtual std::shared_ptr<const ROSMsg> toSrvRequest(const std::optional<TCPROSPacket>& msg) override { 
        int32_t popedCount = 0;
        return toSrvRequest(msg, popedCount);
      }

      virtual std::shared_ptr<const ROSMsg> toSrvResponse(const std::optional<TCPROSPacket>& msg) override { 
        int32_t popedCount = 0;
        return toSrvResponse(msg, popedCount);
      }

      virtual std::shared_ptr<const ROSMsg> toSrvResponse(const std::optional<TCPROSPacket>& msg, int32_t& popedCount) override {
        return responsePacker.toMsg(msg, popedCount);
      }

      virtual std::shared_ptr<const ROSMsg> toSrvRequest(const std::optional<TCPROSPacket>& msg, int32_t& popedCount) override { 
        return requestPacker.toMsg(msg, popedCount);
      }

      virtual std::shared_ptr<TCPROSPacket> requestToPacket(const ROSMsg& msg) override {
        return requestPacker.toPacket(msg);
      }

      virtual std::shared_ptr<TCPROSPacket> responseToPacket(const ROSMsg& msg) override {
        return responsePacker.toPacket(msg);
      }

      virtual std::shared_ptr<ROSMsg> fromJSON(const std::shared_ptr<const JSONObject>& json) override {
        return requestPacker.fromJSON(json);
      }
    };


  }

}