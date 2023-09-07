#pragma once

#include <optional>
#include <string>
#include <vector>

#if (defined(_GLIBCXX_RELEASE) && (_GLIBCXX_RELEASE > 7)) || \
    (defined(_LIBCPP_VERSION) && (_LIBCPP_VERSION > 10000))
#include <filesystem>
namespace fs = std::filesystem;
#else
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;
#endif

std::string ReadFileIntoString(const std::string_view filename);
bool ReadFileIntoString(const std::string_view filename, std::string& contents);
bool SaveWholeFile(const uint8_t* buffer, uint32_t size, const std::string& filename);

bool file_exists(const char* filepath);
bool dir_exists(const char* dirpath);
bool TouchFile(const char* filepath);
size_t GetSize(const fs::path& p);
std::string GetHomeFolder();
std::vector<std::string> GetFilesInDirectory(const std::string& dir);
std::string GetFileExtension(const std::string& FileName);

// Returns just the name of the file, without the path.
std::string GetFilename(const std::string& WholeFile);

std::string GetDirectory(const std::string& WholeFile);
std::string GetParentDirectoryName(const std::string& WholeFile);
bool IsAbsolutePath(const std::string_view filepath);
bool GetAbsolutePath(const std::string_view FilePath, std::string& AbsolutePath,
                     std::optional<std::reference_wrapper<std::error_code>> ErrorCode = std::nullopt);
void TrimString(std::string& str);
void ReplaceStringInPlace(std::string& subject, const std::string& search, const std::string& replace);
std::vector<std::string> FindFilesRecursive(const std::string& start_path,
                                            const std::string& file_extension = "");

bool Match(const char* wild, const char* str);

// Create directory if it doesn't already exist.  Return bool indicating
// existence of directory.
bool CreateDirectory(const std::string& dir);

void test_fds();

namespace toolbox_internal {

fs::path PathConcatImpl(const std::string_view path);

template <typename... Args>
fs::path PathConcatImpl(const std::string_view path, Args... paths) {
  return !path.empty() ? (fs::path{path} / PathConcatImpl(paths...)) : PathConcatImpl(paths...);
}

}  // namespace toolbox_internal

template <typename... Args>
std::string PathConcat(const std::string_view path, Args... paths) {
  fs::path p = toolbox_internal::PathConcatImpl(path, paths...);
  return p.string();
}

template <typename... Args>
std::string FindFileInPaths(const std::string_view generic_path, const std::string_view alternate_path,
                            Args... paths) {
  std::string concat_path = PathConcat(alternate_path, paths...);
  if (generic_path.empty() || file_exists(concat_path.c_str()) || dir_exists(concat_path.c_str())) {
    return concat_path;
  }

  return PathConcat(generic_path, paths...);
}

bool StartsWith(const std::string_view needle, const std::string_view haystack);
bool EndsWith(const std::string_view text, const std::string_view suffix);
std::string GetUserName();
std::string GetExecutablePath();
