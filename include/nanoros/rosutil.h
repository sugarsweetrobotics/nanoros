#pragma once

#include <cstdint>
#include <utility>
#include <string>
#include <optional>

#define ROS_MASTER_DEFAULT_PORT 11311

namespace ssr {

  namespace nanoros {

    std::optional<std::pair<std::string, int32_t>> splitUri(const std::string& uri);

    std::optional<std::pair<std::string, int32_t>> getROSMasterInfo();

    std::string getSelfIP();

    int32_t getEmptyPort(const std::pair<int32_t, int32_t>& range);

    inline int32_t getEmptyPort(const int32_t port_base) {
      return getEmptyPort({port_base, 65535});
    }


  }

}
