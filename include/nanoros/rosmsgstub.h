#pragma once

#include "nanoros/rosmsg.h"
#include <string>
#include <optional>

namespace ssr {
  namespace nanoros {
    class TCPROSPacket;

    class ROSMsgStub {
    public:
      ROSMsgStub() {}
      virtual ~ROSMsgStub() {}

    public:
      virtual std::string md5sum() const { return ""; }
      virtual std::string typeName() const { return ""; }
      virtual std::shared_ptr<const ROSMsg> toMsg(const std::optional<TCPROSPacket>& msg) const { return nullptr; }
    };

  }
}
