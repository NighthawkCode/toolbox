#pragma once

#include <atomic>
#include <chrono>
#include <thread>

class Timer {
  std::atomic<bool> clear_ = false;

public:
  template <typename Function>
  void setTimeout(Function function, int delay) {
    clear_ = false;
    std::thread t([=]() {
      if (clear_) return;
      std::this_thread::sleep_for(std::chrono::milliseconds(delay));
      if (clear_) return;
      function();
    });
    t.detach();
  }

  template <typename Function>
  void setInterval(Function function, int interval) {
    clear_ = false;
    std::thread t([=]() {
      while (true) {
        if (clear_) return;
        std::this_thread::sleep_for(std::chrono::milliseconds(interval));
        if (clear_) return;
        function();
      }
    });
    t.detach();
  }

  void stop() { clear_ = true; }

  template <typename Function>
  static void SetTimeout(Function function, int delay) {
    std::thread t([=]() {
      std::this_thread::sleep_for(std::chrono::milliseconds(delay));
      function();
    });
    t.detach();
  }
};
