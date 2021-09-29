#pragma once

#include <assert.h>

template <class T, int N>
class CircularBuffer {
  T elems[N];
  // How many elements in the array are actually used
  unsigned int used_elems = 0;
  // index of the latest pushed element
  unsigned int latest_index = 0;

public:
  CircularBuffer() = default;

  ~CircularBuffer() {
    used_elems = 0;
    latest_index = 0;
  }

  // The index here goes from 0 (most recent) to size() -1 (oldest)
  T& operator[](unsigned int index) noexcept {
    assert(index < used_elems);
    return elems[(latest_index - index + N) % N];
  }

  // The index here goes from 0 (most recent) to size() -1 (oldest)
  const T& operator[](unsigned int index) const noexcept {
    assert(index < used_elems);
    return elems[(latest_index - index + N) % N];
  }

  // Get the oldest element in the circular buffer
  T& front() noexcept {
    assert(!empty());
    return elems[(latest_index - size() + 1 + N) % N];
  }

  // Get the oldest element in the circular buffer
  const T& front() const noexcept {
    assert(!empty());
    return elems[(latest_index - size() + 1 + N) % N];
  }

  // Get the newest element in the circular buffer (same as [0])
  T& back() noexcept {
    assert(!empty());
    return elems[latest_index];
  }

  // Get the newest element in the circular buffer (same as [0])
  const T& back() const noexcept {
    assert(!empty());
    return elems[latest_index];
  }

  void push_back(T elem) {
    assert(used_elems <= N);
    if (used_elems == N) {
      latest_index = (latest_index + 1) % N;
      elems[latest_index] = elem;
    } else {
      latest_index = used_elems;
      elems[used_elems++] = elem;
    }
  }

  [[nodiscard]] unsigned int size() const noexcept { return used_elems; }
  [[nodiscard]] bool empty() const noexcept { return used_elems == 0; }
  [[nodiscard]] bool full() const noexcept { return used_elems == N; }

  void reset() noexcept {
    used_elems = 0;
    latest_index = 0;
  }
};
