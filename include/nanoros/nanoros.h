#pragma once


namespace ssr {
  namespace nanoros {
    void init_nanoros();
    void spin();
    bool is_shutdown();


    class Duration {
    public:
      const double sec;
    public:
      Duration(const double sec_) : sec(sec_) {}
      ~Duration() {}
    };


    bool sleep_for(const Duration& duration);

  }
}
