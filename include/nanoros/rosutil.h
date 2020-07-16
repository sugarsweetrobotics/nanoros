#pragma once

#include <cstdint>
#include <utility>
#include <optional>

#define ROS_MASTER_DEFAULT_PORT 11311

namespace ssr {

  namespace nanoros {

    std::optional<std::pair<std::string, int32_t>> getROSMasterInfo();
  }

}
