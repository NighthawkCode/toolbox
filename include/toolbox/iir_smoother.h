#pragma once

/// simple infinte impulse response filter
struct iir_smoother {
  double val_;
  double alpha_;

  iir_smoother(double v = 0, double alpha = 0.5) {
    val_ = v;
    alpha_ = alpha;
  }

  double update(double v) {
    val_ = alpha_ * v + (1.0f - alpha_) * val_;
    return val_;
  }

  double operator()(double v) { return update(v); }
};
