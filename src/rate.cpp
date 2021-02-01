#include "rate.h"

void Rate::addPoint(size_t num, double t) {
  if (num != prev_change_.num || prev_change_.t < 0) {
    struct Change c = {num, t};
    changes_[current_index_] = c;
    current_index_++;
    current_index_ = loopedIndex(current_index_);
    prev_change_ = c;
    if (current_index_ == 0) {
      filled_ = true;
    }
  }
}
double Rate::getRate(bool no_divide_by_zero) {
  int cur_index = current_index_ - 1;  // index of last element
  cur_index = loopedIndex(cur_index);
  int index = cur_index;
  if (index == 0 && !filled_) {
    return 0.0;  // same num
  }
  double now = 0.0;
  double now_sum = 0.0;
  double rate = 0.0;
  const double epsilon = 1e-6;
  while (now < window_seconds_) {
    // loop through changes
    index--;  // Go to previous index
    index = loopedIndex(index);

    if (index == cur_index || (!filled_ && index == array_size_ - 1)) {
      if(no_divide_by_zero && now_sum > -epsilon && now_sum < epsilon) {
        return 0.0;
      }
      return rate / now_sum;
    }
    rate += double(changes_[cur_index].num - changes_[index].num);
    now = changes_[cur_index].t - changes_[index].t;  // do next now
    now_sum += now;
  }
  if(no_divide_by_zero && now_sum > -epsilon && now_sum < epsilon) {
    return 0.0;
  }
  rate /= now_sum;
  return rate;
}

int Rate::loopedIndex(int index) { return (index % array_size_ + array_size_) % array_size_; }
