#pragma once


#include "nanoros_define.h"

namespace ssr {
  namespace nanoros {
    NANOROS_API void init_nanoros(const int argc, const char* argv[]);
    NANOROS_API void spin();
    NANOROS_API bool is_shutdown();


    class Duration {
    public:
      const double sec;
    public:
      Duration(const double sec_) : sec(sec_) {}
      ~Duration() {}
    };


    NANOROS_API bool sleep_for(const Duration& duration);

  }
}
