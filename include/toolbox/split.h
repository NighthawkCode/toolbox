#pragma once
#include <string>
#include <vector>

namespace toolbox {

// Split a string opn occurrences of 'sep' character, returning a vector of the strings.
// Zero-length substrings will be returned as empty elements in the vector (i.e.
// split(",,", ',') will return {"", "", ""}).
std::vector<std::string> split(const std::string& s, char seperator);

// Supports comma-separated list of integers, including ranges, e.g.:
// 10         # 10
// 1,2,3      # 1, 2, 3
// 1:3        # 1, 2, 3
// 1,3:8,10   # 1, 3, 4, 5, 6, 7, 8, 10
// -5:-2      # -5, -4, -3, -2

[[nodiscard]] bool split_int_range(const std::string& input, std::vector<int>& output_val);

}  // namespace toolbox
