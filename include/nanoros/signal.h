#pragma once

#include <functional>
#include <utility>

namespace ssr::nanoros {

  enum Signal {
    SIGNAL_INT = 1,
  };

  bool signal(const Signal signal_, void(*handler)(const int));

  bool wait_for(const Signal signal=SIGNAL_INT);
};
