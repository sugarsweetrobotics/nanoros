#pragma once

#include <string>
#include <memory>

#include "nanoros/rosmaster.h"

namespace ssr {
  namespace nanoros {
    class ROSNode;

    class ROSSlaveServer {
    public:
      ROSSlaveServer() {}
      virtual ~ROSSlaveServer() {}

    public:
      virtual std::string getSlaveUri() const = 0;
      virtual std::string getSlaveIP() const = 0;
    };

    std::shared_ptr<ROSSlaveServer> rosslaveserver(ROSNode* rosnode, const std::shared_ptr<ROSMaster>& master, const std::string& selfIp, const int32_t port);
  }
}
