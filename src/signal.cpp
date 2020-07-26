#include <vector>
#include <csignal>
#include <condition_variable>

#include "nanoros/signal.h"



using namespace ssr::nanoros;

namespace
{
  volatile std::sig_atomic_t signal_captured;
  std::condition_variable condition_var;
  std::mutex condition_mutex;
}
 
static void signal_handler(int signal)
{
  signal_captured = signal;
  condition_var.notify_all();
}
 


static int sig2sig(const ssr::nanoros::Signal sig) {
  switch (sig) {
  case ssr::nanoros::Signal::SIGNAL_INT:
    return SIGINT;
  default:
    return 0;
  }
}

bool ssr::nanoros::signal(const Signal signal_, void(*handler)(const int) ) {
  int sig = sig2sig(signal_);
  if (sig) {
    std::signal(sig, handler);
    return true;
  }

  return false;
}

bool ssr::nanoros::wait_for(const ssr::nanoros::Signal signal) {
  int sig = sig2sig(signal);
  if (sig) {
    std::unique_lock<std::mutex> lock(condition_mutex);
    std::signal(sig, signal_handler);
    condition_var.wait(lock);
    return true;
  }

  return false;
}
