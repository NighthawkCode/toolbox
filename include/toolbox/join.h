#pragma once
#include <map>
#include <set>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

// okay, so this is strange but this allows these to work with vectors of strings.
static inline const std::string& to_string(const std::string& str) { return str; }

// For vectors whose elements work with to_string, return a new string
// with the elements joined by spaces.
template <typename T>
std::string join_strings(const std::vector<T>& v, const std::string& sep = " ") {
  std::string result;
  for (auto it = v.begin(); it != v.end();) {
    using namespace std;  // allows either std:: or other namespace for to_string, for custom types
    result += to_string(*it);
    auto next = it;
    next++;
    if (next != v.end()) result += sep;
    it = next;
  }
  return result;
}

template <typename T>
std::string join_strings(const std::set<T>& v, const std::string& sep = " ") {
  std::string result;
  for (auto it = v.begin(); it != v.end();) {
    using namespace std;  // allows either std:: or other namespace for to_string, for custom types
    result += to_string(*it);
    auto next = it;
    next++;
    if (next != v.end()) result += sep;
    it = next;
  }
  return result;
}

template <typename T>
std::string join_strings(const std::unordered_set<T>& v, const std::string& sep = " ") {
  std::string result;
  for (auto it = v.begin(); it != v.end();) {
    using namespace std;  // allows either std:: or other namespace for to_string, for custom types
    result += to_string(*it);
    auto next = it;
    next++;
    if (next != v.end()) result += sep;
    it = next;
  }
  return result;
}

template <typename T, typename T2>
std::string join_strings(const std::unordered_map<T, T2>& m, const std::string& sep = " ") {
  std::string result;
  for (auto it = m.begin(); it != m.end();) {
    using namespace std;  // allows either std:: or other namespace for to_string, for custom types
    result += to_string(it->first) + ":" + to_string(it->second);
    auto next = it;
    next++;
    it = next;
  }
  return result;
}

template <typename T, typename T2>
std::string join_strings(const std::map<T, T2>& m, const std::string& sep = " ") {
  std::string result;
  for (auto it = m.begin(); it != m.end();) {
    using namespace std;  // allows either std:: or other namespace for to_string, for custom types
    result += to_string(it->first) + ":" + to_string(it->second);
    auto next = it;
    next++;
    if (next != m.end()) result += sep;
    it = next;
  }
  return result;
}
