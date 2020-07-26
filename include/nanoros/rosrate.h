#pragma once



namespace ssr {
  namespace nanoros {

    class Rate {
    private:
        const double hz_;
    public:
        Rate(const double hz) : hz_(hz) {

        }
        ~Rate() {}

    public:
        double getRateHz() const { return hz_; }
        double getIntervalSec() const { return 1.0 / hz_; }
        double getIntervalUsec() const { return 1.0E+6 / hz_; }
    };

  }
}