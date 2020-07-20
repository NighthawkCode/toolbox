#pragma once

#include <experimental/filesystem>
#include <optional>
#include <string>
#include <vector>

namespace fs = std::experimental::filesystem;

std::string ReadFileIntoString(const std::string_view filename);
bool ReadFileIntoString(const std::string_view filename, std::string& contents);
bool SaveWholeFile(const uint8_t* buffer, uint32_t size, const std::string& filename);

bool file_exists(const char* filepath);
bool dir_exists(const char* dirpath);
bool TouchFile(const char* filepath);
std::string GetHomeFolder();
std::vector<std::string> GetFilesInDirectory(const std::string& dir);
std::string GetFileExtension(const std::string& FileName);
std::string GetFilename(const std::string& WholeFile);
std::string GetDirectory(const std::string& WholeFile);
bool GetAbsolutePath(const std::string_view FilePath, std::string& AbsolutePath,
                     std::optional<std::reference_wrapper<std::error_code>> ErrorCode = std::nullopt);

// Create directory if it doesn't already exist.  Return bool indicating
// existence of directory.
bool CreateDirectory(const std::string& dir);

void test_fds();

namespace internal {

fs::path PathConcatImpl(const std::string_view path);

template <typename... Args>
fs::path PathConcatImpl(const std::string_view path, Args... paths) {
  return !path.empty() ? (fs::path{path} / PathConcatImpl(paths...)) : PathConcatImpl(paths...);
}

}  // namespace internal

template <typename... Args>
std::string PathConcat(const std::string_view path, Args... paths) {
  fs::path p = internal::PathConcatImpl(path, paths...);
  return fs::absolute(p).string();
}
