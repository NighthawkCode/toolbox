#pragma once

#include <assert.h>

#include <list>
#include <memory>
#include <unordered_map>

/// This is a class to have LRU of elements
/// Templated in a key/value store, it keeps track
/// of the most used elements and will free the others.
/// This class should hold the memory via the shared ptr
/// The memory is freed automatically when too many insertions are made.
///
/// There are functions to change the allowed size, and to even
/// query the limits, the next one to be evicted, etc.
/// This class does not do any multithreaded checking, this is left
/// up to the parent container.

template <typename Key, typename Value>
class LRUCache {
  using ValuePtr = std::shared_ptr<Value>;
  using ListType = std::list<std::pair<Key, ValuePtr>>;
  ListType dq;
  std::unordered_map<Key, typename ListType::iterator> ma;
  size_t csize;  // maximum number of elements to cache

  void evict() {
    if (dq.empty()) return;

    // delete least recently used element
    auto [lastk, lastv] = dq.back();

    // Pops the last elmeent
    dq.pop_back();

    // Erase the last
    ma.erase(lastk);
  }

public:
  LRUCache(size_t capacity = 80)
      : csize(capacity) {}

  // This query will obtain a value from the cache and be taken
  // into account for LRU purposes
  ValuePtr query(const Key& k) {
    // not present in cache
    if (ma.find(k) == ma.end()) {
      return ValuePtr();
    }  // present in cache
    else {
      dq.push_front(*ma[k]);
      dq.erase(ma[k]);
      ma[k] = dq.begin();
      return dq.begin()->second;
    }
  }

  // This function will check if a key is in the cache, but
  // without using this query for LRU purposes
  bool present(const Key& k) {
    if (ma.find(k) == ma.end()) {
      return false;
    }  // present in cache
    else {
      return true;
    }
  }

  // This function inserts a value in the cahce
  void insert(const Key& k, ValuePtr v) {
    // not present in cache
    if (ma.find(k) == ma.end()) {
      // cache is full
      if (dq.size() == csize) {
        // delete least recently used element
        evict();
      }
    }  // present in cache
    else {
      dq.erase(ma[k]);
    }

    // update reference
    dq.emplace_front(k, v);
    ma[k] = dq.begin();
  }

  void insert(const Key& k, const Value& v) {
    insert(k, std::make_shared<Value>(v));
  }

  // This function will remove a key,value from the cache
  // if it was there
  void erase(const Key&k) {
    if (ma.find(k) == ma.end()) {
      return;
    }
    dq.erase(ma[k]);
    ma.erase(k);
  }

  bool empty() const { return dq.empty(); }
  bool full() const { return dq.size() == csize; }
  size_t size() const { return dq.size(); }
  // This function returns the value that would be evicted next
  ValuePtr toEvict() { return dq.back().second; }
  ValuePtr evictAndReturnLast() {
    ValuePtr v = toEvict();
    evict();
    return v;
  }

  // Change to allow now 'capacity' elements
  void resize(size_t capacity) {
    csize = capacity;
    while (dq.size() > csize) {
      evict();
    }
  }

  // for debugging
  auto begin() const {
      return dq.begin();
  }
  auto end() const {
      return dq.end();
  }

};
