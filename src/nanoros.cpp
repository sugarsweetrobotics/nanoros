#include "nanoros/nanoros.h"
#include "aqua2/socket.h"
#include "nanoros/signal.h"
#include "nanoros/os.h"

#include "nanoros/rosmsgstubfactory.h"
#include "nanoros/rossrvstubfactory.h"

#include <thread>

namespace {

  volatile bool shutdown_flag = false;

  void signal_handler(const int32_t sig) {
    std::cout << "nanoros:: signal_handler" << std::endl;
    shutdown_flag = true;
  }
}

void ssr::nanoros::init_nanoros(const int argc, const char* argv[]) {
  ssr::aqua2::initializeSocket();

  ssr::nanoros::signal(ssr::nanoros::SIGNAL_INT, signal_handler);

  auto absPath = getExecutablePath(argv[0]);


  getROSMsgStubFactory()->addStubDirHint(absPath);
  getROSMsgStubFactory()->addStubDirHint(absPath + "../share/nanoros/stubs/");
#ifdef WIN32
  getROSMsgStubFactory()->addStubDirHint(absPath + "../../share/nanoros/stubs/");
#else

#endif
}


void ssr::nanoros::spin() {
    /// TODO: This must be changed to wait_for method ...
    while(!is_shutdown()) {
      std::this_thread::sleep_for(std::chrono::milliseconds(1));      
    }
}

bool ssr::nanoros::is_shutdown() {
  return shutdown_flag;
}

bool ssr::nanoros::sleep_for(const ssr::nanoros::Duration& duration) {
  uint64_t nanosec = duration.sec * 1.0E+9;
  std::this_thread::sleep_for(std::chrono::nanoseconds(nanosec));
  return true;
}
