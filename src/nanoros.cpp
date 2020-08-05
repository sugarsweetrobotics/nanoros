#include "nanoros/nanoros.h"
#include "aqua2/socket.h"
#include "nanoros/signal.h"
#include "nanoros/os.h"

#include "nanoros/rosmsgpackerfactory.h"
#include "nanoros/rossrvpackerfactory.h"

#include "nanoros/argparse.h"
#include "nanoros/stringutil.h"

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

  ssr::nanoros::ArgParser parser;
  // parser.option()

  ssr::nanoros::signal(ssr::nanoros::SIGNAL_INT, signal_handler);

  auto absPath = getExecutablePath(argv[0]);


  getROSMsgPackerFactory()->addPackerDirHint(absPath);
  auto packerDir = ssr::nanoros::getEnv("NANOROS_PACKER_DIR");
  if (packerDir.length() > 0) {
      if (packerDir.rfind('/') != packerDir.length() - 1) {
          packerDir += '/';
      }
      getROSMsgPackerFactory()->addPackerDirHint(packerDir);

  }

  auto packerDirs = ssr::nanoros::getEnv("NANOROS_PACKER_DIRS");
  if (packerDirs.length() > 0) {
#ifdef WIN32
      const char sep = ';';
#else
      const char sep = ':';
#endif
      auto dirs = ssr::nanoros::stringSplit(packerDirs, sep);
      for (auto dir : dirs) {
          if (dir.rfind('/') != dir.length() - 1) {
              dir += '/';
          }
          getROSMsgPackerFactory()->addPackerDirHint(dir);
      }
  }


  getROSMsgPackerFactory()->addPackerDirHint(absPath + "../share/nanoros/packers/");
#ifdef WIN32
  getROSMsgPackerFactory()->addPackerDirHint(absPath + "../../share/nanoros/packers/");
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
