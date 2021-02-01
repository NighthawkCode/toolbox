#pragma once

#include <assert.h>

template <class T, int N>
class CircularBuffer {
  T elems[N];
  unsigned int used_elems = 0;
  unsigned int latest_index = 0;

public:
  CircularBuffer() = default;

  ~CircularBuffer() {
    used_elems = 0;
    latest_index = 0;
  }

  T& operator[](unsigned int index) {
    assert(index < used_elems);
    return elems[(index + latest_index) % N];
  }
  const T& operator[](unsigned int index) const {
    assert(index < used_elems);
    return elems[(index + latest_index) % N];
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

  unsigned int size() const { return used_elems; }
  void reset() {
    used_elems = 0;
    latest_index = 0;
  }
};
