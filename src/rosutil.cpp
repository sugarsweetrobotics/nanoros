#include "aqua2/socket.h"
#include "aqua2/serversocket.h"


#include "nanoros/os.h"

#include "nanoros/rosutil.h"
#include "nanoros/stringutil.h"


#include <iostream>
#include <random>

std::optional<std::pair<std::string, int32_t>> ssr::nanoros::splitUri(const std::string& uri_) {
  std::string uri = uri_;
  if (uri.find("://") == std::string::npos) {
    //return std::nullopt;
  } else {
    auto substrs = stringSplit(uri, '/');
    uri = substrs[2];
  }
  auto vs = ssr::nanoros::stringSplit(uri, ':');
  if (vs.size() == 2) {
    return std::make_pair(vs[0], atoi(vs[1].c_str()));
  } else {
    return std::make_pair(vs[0], ROS_MASTER_DEFAULT_PORT);
  }

}

std::optional<std::pair<std::string, int32_t>> ssr::nanoros::getROSMasterInfo() {
    auto uri = ssr::nanoros::getEnv("ROS_MASTER_URI");
    if (uri.length() == 0) {
        return std::make_pair<std::string, int32_t>("127.0.0.1", 11311);
//        return std::nullopt;
    }
    return ssr::nanoros::splitUri(uri);
}

std::string ssr::nanoros::getSelfIP() {
  auto ip = ssr::nanoros::getEnv("ROS_IP");
  if (ip.length() == 0) {
      return "127.0.0.1";
  }
  return ip;
}

int32_t ssr::nanoros::getEmptyPort(const std::pair<int32_t, int32_t>& range) {
  try {
    std::mt19937 mt{ std::random_device{}() };
    std::uniform_int_distribution<int> dist(range.first, range.second);
    
    while (true) {
      try {
        int port = dist(mt);
        ssr::aqua2::ServerSocket ss;
        ss.bind(port);
        ss.close();
        return port;
        //break;
      } catch (ssr::aqua2::SocketException& bind) {
	
      }
    }
  } catch (std::exception& e) {
    return -1;
  }
}
