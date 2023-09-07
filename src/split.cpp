#include "toolbox/split.h"

#include <optional>
namespace toolbox {

std::vector<std::string> split(const std::string& s, char seperator) {
  std::vector<std::string> output;

  if (s.empty()) {
    return {""};
  }
  std::string::size_type prev_pos = 0, pos = 0;

  while ((pos = s.find(seperator, pos)) != std::string::npos) {
    std::string substring(s.substr(prev_pos, pos - prev_pos));
    output.push_back(substring);
    prev_pos = ++pos;
  }

  output.push_back(s.substr(prev_pos, s.size() - prev_pos));  // Last word
  return output;
}

template <typename T>
static std::optional<int> string_to_int_with_check(const T& str) {
  int value = atoi(str.c_str());
  if (value == 0) {
    // atoi will return 0 if a string is not a valid integer.  So we need to check
    // that case here.
    if (str.empty()) return {};
    int start = 0;
    // ingest leading whitespace
    for (; start < (int)str.size() && isspace(str[start]); start++)
      ;
    int end = (int)str.size() - 1;
    // string trailing whitespace
    for (; end >= 0 && isspace(str[end]); end--)
      ;

    bool found_zero = false;
    for (int i = start; i <= end; i++) {
      const auto& c = str[i];
      if (i == start && (c == '+' || c == '-')) {
        // allow +/- as first character only
        continue;
      }
      if (c == '0' && !found_zero) {
        found_zero = true;
      } else {
        // non-zero character or multiple zeros--fail
        return {};
      }
    }
    if (!found_zero) return {};
  }
  return {value};
}

// Supports comma-separated list of integers, including ranges, e.g.:
// 10         # 10
// 1,2,3      # 1, 2, 3
// 1:3        # 1, 2, 3
// 1,3:8,10   # 1, 3, 4, 5, 6, 7, 8, 10
// -5:-2      # -5, -4, -3, -2

bool split_int_range(const std::string& input, std::vector<int>& output_val) {
  auto split_str = split(input, ',');
  for (auto& s : split_str) {
    if (size_t sep_pos = s.find(':'); sep_pos != std::string::npos) {
      if (sep_pos == 0 || sep_pos == s.size() - 1) {
        return false;
      }
      const auto start = string_to_int_with_check(s.substr(0, sep_pos));
      const auto end = string_to_int_with_check(s.substr(sep_pos + 1, s.size() - sep_pos - 1));
      if (!start || !end) return false;
      if (*end <= *start) {
        return false;
      }
      for (int i = *start; i <= *end; i++) {
        output_val.push_back(i);
      }
    } else {
      const auto val = string_to_int_with_check(s);
      if (!val) return false;
      output_val.push_back(*val);
    }
  }
  return true;
}

}  // namespace toolbox
