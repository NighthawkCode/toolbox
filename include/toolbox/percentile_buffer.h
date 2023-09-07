/**
 * @file percentile_buffer.h
 * @brief A FIFO buffer supporting sorted insertions and efficient access to a specified percentile of its
 * elements.
 * @author Dhaivat Dholakiya
 * Copyright (2023), Verdant Robotics
 */
#pragma once

#include <cassert>
#include <iostream>
#include <map>

/**
 * @class PercentileBuffer
 * @brief A buffer supporting sorted insertions and efficient access to a specified percentile of its
 * elements.
 *
 * @note
 * - The goal of this class is to efficiently handle the following operations:
 *     - Adding a double value to the buffer.
 *     - Providing the specified percentile of the current values in the buffer.
 * - Inserts values while maintaining a sorted order.
 * - The 'add' operation is designed with a time complexity of O(log n).
 * - The 'getPercentile' operation has a time complexity of O(1), allowing efficient access to the specified
 * percentile. The percentile is specified in the constructor and defaults to 0.95.
 * - Ideal for applications where continuous calculation and access of a specific percentile of a data-set is
 * required.
 *
 * @note The buffer is implemented using two multimaps, one for the left half of the buffer and one for the
 * right half. The left half is sorted in descending order and the right half is sorted in ascending order.
 * This allows for efficient access to the specified percentile element.
 */
template <typename T>
class PercentileBuffer {
private:
  std::multimap<T, int> left, right;  ///< Multimaps to store the elements.
  size_t sequence;                    ///< Sequence number for the next element to be added.
  size_t max_size;                    ///< Maximum size of the buffer.
  double percentile;                  ///< The percentile to be calculated. Defaults to 0.95.

public:
  /**
   * @brief Constructor that initializes the buffer with a maximum size and a percentile.
   * @param max_size The maximum size of the buffer.
   * @param percentile The percentile to be calculated.
   */
  PercentileBuffer(size_t max_size, double percentile = 0.95)
      : sequence(0)
      , max_size(max_size)
      , percentile(percentile) {
    assert(max_size > 0);
    assert(percentile >= 0.0 && percentile <= 1.0);
  }

  /**
   * @brief Adds a new element to the buffer.
   * @param value The value of the new element.
   * @note Time complexity is O(log n).
   */
  void add(const T& value) noexcept {
    // Check if buffer is full before adding new element
    if (full()) {
      // Remove the oldest element
      if (left.begin()->first < right.begin()->first) {
        left.erase(left.begin());
      } else {
        right.erase(right.begin());
      }
    }

    if (right.empty()) {
      left.insert({value, sequence++});
    } else if (value < right.begin()->first) {
      left.insert({value, sequence++});
    } else {
      right.insert({value, sequence++});
    }

    // Rebalance if necessary
    while (left.size() < size() * percentile) {
      auto it = right.begin();
      left.insert({it->first, it->second});
      right.erase(it);
    }
    while (!left.empty() && left.size() > size() * percentile) {
      auto it = --left.end();  // safe to use --end() as left is not empty
      right.insert({it->first, it->second});
      left.erase(it);
    }
  }

  /**
   * @brief Returns the specified percentile element in the buffer.
   * @return The specified percentile element.
   * @note Time complexity is O(1).
   */
  const T getPercentile() const noexcept {
    T pv = T();

    // return empty value if buffer is empty
    if (empty()) {
      return pv;
    }

    double K = size() * percentile;

    if (left.size() >= K) {
      // If left container holds more than the specified percentile of the total values,
      // the percentile is the maximum value in the left container.
      pv = (--left.end())->first;
    } else {
      // If left container holds less than the specified percentile of the total values,
      // the percentile is the minimum value in the right container.
      pv = right.begin()->first;
    }

    // default return value is 0
    return pv;
  }

  /**
   * @brief Returns the size of the buffer.
   * @return The size of the buffer.
   */
  [[nodiscard]] size_t size() const noexcept { return left.size() + right.size(); }

  // max size
  /**
   * @brief Returns the maximum size of the buffer.
   * @return The maximum size of the buffer.
   */
  [[nodiscard]] size_t maxSize() const noexcept { return max_size; }

  /**
   * @brief Returns the current index of the buffer.
   * @return The current index of the buffer.
   */
  [[nodiscard]] size_t currentIndex() const noexcept { return sequence; }

  /**
   * @brief Checks if the buffer is empty.
   * @return True if the buffer is empty, false otherwise.
   */
  [[nodiscard]] bool empty() const noexcept { return left.empty() && right.empty(); }

  /**
   * @brief Checks if the buffer is full.
   * @return True if the buffer is full, false otherwise.
   */
  [[nodiscard]] bool full() const noexcept { return size() >= max_size; }

  /**
   * @brief Resets the buffer, clearing all elements and resetting the sequence number.
   */
  void reset() noexcept {
    left.clear();
    right.clear();
    sequence = 0;
  }

  /**
   * @brief Checks if a given value is in the buffer.
   * @param value The value to find.
   * @return The index of the value in the buffer if it exists, -1 otherwise.
   */
  int find(T value) const noexcept {
    int index = 0;
    for (const auto& pair : left) {
      if (pair.first == value) {
        return index;
      }
      ++index;
    }
    for (const auto& pair : right) {
      if (pair.first == value) {
        return index;
      }
      ++index;
    }
    return -1;  // Value not found
  }

  /**
   * @brief Returns the percentile parameter.
   * @return The percentile parameter.
   */
  [[nodiscard]] double getPercentileParameter() const noexcept { return percentile; }

  /**
   * @brief Re-initializes the buffer with a new percentile parameter.
   * @param percentile The new percentile parameter.
   */
  void setPercentileParameter(double value) noexcept {
    // return if percentile is the same
    if (percentile == value) {
      return;
    }

    std::cout << "PercentileBuffer: Changing percentile from " << percentile << " to " << value << std::endl;

    // Check if percentile is in range
    assert(value >= 0.0 && value <= 1.0);

    // Update percentile
    percentile = value;

    // Rebalance if necessary
    while (left.size() < size() * percentile) {
      auto it = right.begin();
      left.insert({it->first, it->second});
      right.erase(it);
    }
    while (!left.empty() && left.size() > size() * percentile) {
      auto it = --left.end();  // safe to use --end() as left is not empty
      right.insert({it->first, it->second});
      left.erase(it);
    }
  }

  // iterators

  /**
   * @brief Returns an iterator to the beginning of the buffer.
   * @return An iterator to the beginning of the buffer.
   */
  typename std::multimap<T, int>::iterator begin() noexcept { return left.begin(); }

  /**
   * @brief Returns a const iterator to the beginning of the buffer.
   * @return A const iterator to the beginning of the buffer.
   */
  typename std::multimap<T, int>::const_iterator begin() const noexcept { return left.begin(); }

  /**
   * @brief Returns an iterator to the end of the buffer.
   * @return An iterator to the end of the buffer.
   */
  typename std::multimap<T, int>::iterator end() noexcept { return right.end(); }

  /**
   * @brief Returns a const iterator to the end of the buffer.
   * @return A const iterator to the end of the buffer.
   */
  typename std::multimap<T, int>::const_iterator end() const noexcept { return right.end(); }

  /**
   * @brief Advances an iterator by n positions.
   * @param it The iterator to advance.
   * @param n The number of positions to advance.
   * @note Time complexity is O(N) for large jumps. O(1) for increment by 1
   */
  void advance(typename std::multimap<T, int>::iterator& it, int n) noexcept {
    while (n > 0) {
      ++it;
      if (it == left.end()) {
        it = right.begin();
      }
      --n;
    }
    while (n < 0) {
      if (it == right.begin()) {
        it = left.end();
      }
      --it;
      ++n;
    }
  }

  /**
   * @brief Advances a const iterator by n positions.
   * @param it The const iterator to advance.
   * @param n The number of positions to advance.
   * @note Time complexity is O(N) for large jumps. O(1) for increment by 1
   */
  void advance(typename std::multimap<T, int>::const_iterator& it, int n) const noexcept {
    while (n > 0) {
      ++it;
      if (it == left.cend()) {
        it = right.cbegin();
      }
      --n;
    }
    while (n < 0) {
      if (it == right.cbegin()) {
        it = left.cend();
      }
      --it;
      ++n;
    }
  }

  /**
   * @brief A convienience function to advance an iterator by one position.
   * @param it The iterator to increment.
   * @note Time complexity is O(1)
   */
  void next(typename std::multimap<T, int>::iterator& it) noexcept { advance(it, 1); }

  /**
   * @brief A convienience function to advance a const iterator by one position.
   * @param it The iterator to increment.
   * @note Time complexity is O(1)
   */
  void next(typename std::multimap<T, int>::const_iterator& it) const noexcept { advance(it, 1); }

  /**
   * @brief A convienience function to decrement an iterator by one position.
   * @param it The iterator to decrement.
   * @note Time complexity is O(1)
   */
  void previous(typename std::multimap<T, int>::iterator& it) noexcept { advance(it, -1); }

  /**
   * @brief A convienience function to decrement a const iterator by one position.
   * @param it The iterator to decrement.
   * @note Time complexity is O(1)
   */
  void previous(typename std::multimap<T, int>::const_iterator& it) const noexcept { advance(it, -1); }
};
