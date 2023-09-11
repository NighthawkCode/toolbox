#pragma once

#include <assert.h>

#include <vector>

// Base class for circular buffer. Relies on 'elems' having an array subscript operator.
// Size of elems should be stored in N.  See the two derived classes following this definition.
template <class T, typename T_storage>
class CircularBufferBase {
  // How many elements in the array are actually used
  unsigned int used_elems = 0;
  // index of the latest pushed element
  unsigned int latest_index = 0;

protected:
  T_storage elems;

  unsigned int N = 0;

public:
  CircularBufferBase(unsigned int num_elems)
      : N(num_elems) {}

  ~CircularBufferBase() {
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

  // Reverse order access, index = 0 means the oldest element
  T& rorder(unsigned int index) noexcept {
    assert(!empty());
    return elems[(latest_index - size() + 1 + N + index) % N];
  }

  const T& rorder(unsigned int index) const noexcept {
    assert(!empty());
    return elems[(latest_index - size() + 1 + N + index) % N];
  }

  // Remove <num> oldest elements from the buffer
  void purge(unsigned int num) noexcept {
    if (num < used_elems) {
      used_elems -= num;
    } else {
      used_elems = 0;
    }
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

  void push_back(const T& elem) noexcept {
    assert(used_elems <= N);
    latest_index = (latest_index + 1) % N;
    if (used_elems != N) {
      used_elems++;
    }
    elems[latest_index] = elem;
  }

  // Add an element to the newest on the buffer
  T& emplace_back() noexcept {
    assert(used_elems <= N);
    latest_index = (latest_index + 1) % N;
    if (used_elems != N) {
      used_elems++;
    }
    return elems[latest_index];
  }

  [[nodiscard]] unsigned int size() const noexcept { return used_elems; }
  [[nodiscard]] bool empty() const noexcept { return used_elems == 0; }
  [[nodiscard]] bool full() const noexcept { return used_elems == N; }

  void reset() noexcept {
    used_elems = 0;
    latest_index = 0;
  }
};

// Statically-sized circular buffer (i.e. allocated at compile time)
template <typename T, size_t N_elems>
class CircularBuffer : public CircularBufferBase<T, T[N_elems]> {
public:
  CircularBuffer()
      : CircularBufferBase<T, T[N_elems]>(N_elems) {}
};

// Dynamically-sized circular buffer (i.e. allocated at runtime)
template <typename T>
class CircularBufferVar : public CircularBufferBase<T, std::vector<T>> {
public:
  // Constructor for the indicated number of elements.
  CircularBufferVar(unsigned int max_elems)
      : CircularBufferBase<T, std::vector<T>>(max_elems) {
    CircularBufferBase<T, std::vector<T>>::elems.resize(max_elems);
  }

  // Resize storage; also clears buffer.
  void resize(unsigned int max_elems) {
    CircularBufferBase<T, std::vector<T>>::N = max_elems;
    CircularBufferBase<T, std::vector<T>>::elems.resize(max_elems);
    CircularBufferBase<T, std::vector<T>>::reset();
  }

  // Return the maximum size of the buffer.
  size_t max_size() const { return CircularBufferBase<T, std::vector<T>>::elems.size(); }
};
