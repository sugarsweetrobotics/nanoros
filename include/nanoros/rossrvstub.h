#pragma once

#include "nanoros/rossrv.h"
#include "nanoros/json.h"
#include "nanoros/rostcpros.h"
#include "nanoros/rostcprospacket.h"
#include "nanoros/rosmsgstub.h"

#include <string>
#include <map>
#include <optional>

namespace ssr {
  namespace nanoros {


    class TCPROSPacket;

    class ROSSrvStub {
    protected:
      std::map<std::string, std::shared_ptr<ROSMsgStub>> stubs_;

    public:
      std::shared_ptr<ROSMsgStub> getMsgStub(const std::string& key);

    public:
      ROSSrvStub() {}
      virtual ~ROSSrvStub() {}

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