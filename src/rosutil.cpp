#include <iostream>

#include "nanoros/rosutil.h"
#include "nanoros/stringutil.h"
#include "nanoros/os.h"




std::optional<std::pair<std::string, int32_t>> ssr::nanoros::getROSMasterInfo() {
  auto uri = ssr::nanoros::getEnv("ROS_MASTER_URI");
  if (uri.find("http://") != 0) {
    return std::nullopt;
  } else {
    uri = uri.substr(strlen("http://"));
  }
  auto vs = ssr::nanoros::stringSplit(uri, ':');
  if (vs.size() == 2) {
    return std::make_pair(vs[0], atoi(vs[1].c_str()));
  } else {
    return std::make_pair(vs[0], ROS_MASTER_DEFAULT_PORT);
  }
}
