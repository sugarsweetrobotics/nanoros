#include "nanoros/rosutil.h"
#include "nanoros/os.h"



std::pair<std::string, int32_t> ssr::nanoros::getROSMasterInfo() {
  auto vs = ssr::nanoros::stringSplit(ssr::nanoros::getEnv("ROS_MASTER_URI"), ':');
  if (vs.size() == 2) {
    return {vs[0], atoi(vs[1].c_str())};
  } else {
    return {vs[0], ROS_MASTER_DEFAULT_PORT};
  }
}
