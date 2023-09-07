#pragma once
#include <unistd.h>

#include <array>

class Rate {
public:
  Rate(double window_seconds = 5.0) { window_seconds_ = window_seconds; }
  void addPoint(size_t num, double t);
  double getRate(bool no_divide_by_zero = true) const;

private:
  struct Change {
    size_t num = 0;
    double t = 0;
  };
  static constexpr int array_size_ = 100;
  int current_index_ = 0;
  double window_seconds_;
  std::array<Change, array_size_> changes_;
  Change prev_change_ = {0, -1};
  bool filled_ = false;

  int loopedIndex(int index) const;
};
