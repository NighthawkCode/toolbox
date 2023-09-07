#pragma once

#include <string>
#include <vector>

namespace toolbox {

/**
 * @brief Format a string using printf-style formatting. Uses std::format() if available.
 */
std::string fmt(const char* fmt, ...) __attribute__((__format__(__printf__, 1, 0)));

/**
 * @brief Return true if `str` includes `substr`.
 */
bool StrContains(const std::string& str, const std::string& substr);

/**
 * @brief Convert a buffer (char[], uint8_t[]) to a string, stopping at the first null character or
 * maxLength.
 */
std::string BufferToString(const void* buffer, size_t maxLength);

/**
 * Copy a string to a buffer (char[], uint8_t[]), stopping at the first null character or maxLength.
 */
void StringToBuffer(const std::string& str, void* buffer, size_t maxLength);

/**
 * Remove trailing numbers from a string
 */
std::string removeTrailingNumbers(const std::string& str);

/**
 * @brief Join a vector of strings into a single string, separated by `joiner`.
 */

std::string join(const std::vector<std::string>& buf, std::string_view joiner);
}  // namespace toolbox
