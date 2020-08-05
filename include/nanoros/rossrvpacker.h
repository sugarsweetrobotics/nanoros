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

    class NANOROS_API  ROSSrvPacker {
    protected:
      std::map<std::string, std::shared_ptr<ROSMsgPacker>> stubs_;

    public:
      std::shared_ptr<ROSMsgPacker> getMsgPacker(const std::string& key);

    public:
      ROSSrvPacker() {}
      virtual ~ROSSrvPacker() {}

    public:
      virtual std::string md5sum() const { return ""; }
      virtual std::string typeName() const { return ""; }

      virtual std::shared_ptr<const ROSSrvRequest> toSrvRequest(const std::optional<TCPROSPacket>& msg) { 
        int32_t popedCount = 0;
        return toSrvRequest(msg, popedCount);
      }

      virtual std::shared_ptr<const ROSSrvResponse> toSrvResponse(const std::optional<TCPROSPacket>& msg) { 
        int32_t popedCount = 0;
        return toSrvResponse(msg, popedCount);
      }

      virtual std::shared_ptr<const ROSSrvResponse> toSrvResponse(const std::optional<TCPROSPacket>& msg, int32_t& popedCount) { return nullptr; }
      virtual std::shared_ptr<const ROSSrvRequest> toSrvRequest(const std::optional<TCPROSPacket>& msg, int32_t& popedCount) { return nullptr; }
      virtual std::shared_ptr<TCPROSPacket> toPacket(const ROSSrvRequest& msg) {return nullptr; }
      virtual std::shared_ptr<TCPROSPacket> toPacket(const ROSSrvResponse& msg) {return nullptr; }
      virtual std::shared_ptr<ROSSrvRequest> fromJSON(const std::shared_ptr<const JSONObject> json) { return nullptr; }
    };


  }

}