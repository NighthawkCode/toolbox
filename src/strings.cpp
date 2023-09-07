#include <cstdio>
#include <cstring>

#include "string_utils.h"

#if __has_include(<format>)
#include <format>
#endif

namespace toolbox {

std::string fmt(const char* fmt, ...) {
  // Use std::format if available
#if __has_include(<format>)
  va_list args;
  va_start(args, fmt);
  const auto result = std::vformat(fmt, args);
  va_end(args);
  return result;
#else
  // Otherwise, use vsnprintf
  va_list args;
  va_start(args, fmt);

  va_list vacount;
  va_copy(vacount, args);
  char small[10];
  const int size = vsnprintf(small, 0, fmt, vacount) + 1;
  va_end(vacount);

  std::string result(size, '\0');
  vsnprintf(result.data(), size, fmt, args);
  va_end(args);

  // Remove trailing \0
  result.resize(size - 1);

  return result;
#endif
}

bool StrContains(const std::string& str, const std::string& substr) {
  return str.find(substr) != std::string::npos;
}

std::string BufferToString(const void* buffer, size_t maxLength) {
  const char* charPtr = reinterpret_cast<const char*>(buffer);
  const size_t len = strnlen(charPtr, maxLength);
  return (len > 0) ? std::string{charPtr, len} : std::string{};
}

void StringToBuffer(const std::string& str, void* buffer, size_t maxLength) {
  // Based on the BSD-licensed implementation of strlcpy()
  // Copyright (c) 1998 Todd C. Miller <Todd.Miller@courtesan.com>
  if (maxLength == 0) {
    return;
  }

  char* d = reinterpret_cast<char*>(buffer);
  const char* s = str.c_str();
  size_t n = maxLength;

  // Copy as many bytes as will fit
  while (--n != 0) {
    if ((*d++ = *s++) == '\0') break;
  }

  if (n == 0) {
    *d = '\0';  // NUL-terminate dst
  }
}

std::string removeTrailingNumbers(const std::string& str) {
  ssize_t i = str.length() - 1;
  while (i >= 0 && isdigit(str[i])) {
    i--;
  }
  return std::string(str.begin(), str.begin() + i + 1);
}

std::string join(const std::vector<std::string>& buf, std::string_view joiner) {
  if (buf.empty()) {
    return "";
  }
  size_t size = (buf.size() - 1) * joiner.size();
  for (const std::string& s : buf) {
    size += s.size();
  }

  std::string out;
  out.reserve(size);
  out = buf[0];

  for (size_t i = 1; i < buf.size(); i++) {
    out += joiner;
    out += buf[i];
  }

  return out;
}
}  // namespace toolbox
