#include "file_utils.h"

#ifdef _WIN32
#include <windows.h>  // GetModuleFileNameW
#else
#include <limits.h>  // PATH_MAX
#include <unistd.h>  // readlink
#endif

#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/statvfs.h>
#include <sys/types.h>

#include <fstream>
#include <streambuf>

#include "vlog.h"

bool StartsWith(const std::string_view needle, const std::string_view haystack) {
  return haystack.rfind(needle, 0) == 0;
}

bool EndsWith(const std::string_view text, const std::string_view suffix) {
  if (text.length() >= suffix.length()) {
    return (0 == text.compare(text.length() - suffix.length(), suffix.length(), suffix));
  }
  return false;
}

std::string ReadFileIntoString(const std::string_view filename) {
  std::string contents;
  ReadFileIntoString(filename, contents);
  return contents;
}

bool ReadFileIntoString(const std::string_view filename, std::string& buf) {
  if (!file_exists(filename.data())) {
    vlog_error(VCAT_GENERAL, "ReadFileIntoString: '%s' is not a file or does not exist", filename.data());
    return false;
  }
  std::ifstream t(filename.data());
  if (!t.is_open()) {
    vlog_error(VCAT_GENERAL, "ReadFileIntoString: Could not open file %s for reading", filename.data());
    return false;
  }

  t.seekg(0, std::ios::end);
  buf.reserve(t.tellg());
  t.seekg(0, std::ios::beg);
  buf.assign((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
  return t.good();
}

void ReplaceStringInPlace(std::string& subject, const std::string& search, const std::string& replace) {
  size_t pos = 0;
  while ((pos = subject.find(search, pos)) != std::string::npos) {
    subject.replace(pos, search.length(), replace);
    pos += replace.length();
  }
}

bool SaveWholeFile(const uint8_t* buffer, uint32_t size, const std::string& filename) {
  FILE* file = fopen(filename.c_str(), "wb");
  if (file) {
    fwrite(buffer, size, 1, file);
    fclose(file);  // close the file
    return true;
  }
  return false;
}

bool file_exists(const char* filepath) {
  struct stat status;
  if (stat(filepath, &status) == 0 && S_ISREG(status.st_mode)) {
    return true;
  }
  return false;
}

bool dir_exists(const char* dirpath) {
  struct stat status;
  if (stat(dirpath, &status) == 0 && S_ISDIR(status.st_mode)) {
    return true;
  }
  return false;
}

size_t GetSize(const fs::path& p) {
  if (fs::is_regular_file(p)) {
    return fs::file_size(p);
  } else if (fs::is_directory(p)) {
    size_t total = 0;
    for (auto& it : fs::directory_iterator(p)) {
      total += GetSize(it);
    }
    return total;
  } else {
    return 0;
  }
}

bool TouchFile(const char* filepath) {
  int fd = open(filepath, O_WRONLY | O_CREAT | O_NOCTTY | O_NONBLOCK, 0666);
  if (fd < 0) {
    return false;
  }
  close(fd);
  return true;
}

std::string GetHomeFolder() {
  const char* home_dir = nullptr;
  home_dir = getenv("HOME");
  if (home_dir == nullptr) {
    home_dir = getpwuid(getuid())->pw_dir;
  }
  return std::string(home_dir);
}

std::vector<std::string> GetFilesInDirectory(const std::string& dir) {
  std::vector<std::string> vec;

  DIR* dirp = opendir(dir.c_str());
  if (dirp == nullptr) return vec;

  struct dirent* dp = readdir(dirp);

  while (dp != nullptr) {
    vec.push_back(std::string(dp->d_name));
    dp = readdir(dirp);
  }
  closedir(dirp);

  return vec;
}

std::string GetFileExtension(const std::string& FileName) {
  if (FileName.find_last_of(".") != std::string::npos) return FileName.substr(FileName.find_last_of(".") + 1);
  return "";
}

std::string GetFilename(const std::string& WholeFile) {
  std::string ret = WholeFile;
  if (ret.back() == '/') {
    ret.pop_back();
  }
  auto slash_pos = ret.find_last_of("/");
  if (slash_pos != std::string::npos) {
    return ret.substr(slash_pos + 1);
  }
  return ret;
}

std::string GetDirectory(const std::string& WholeFile) {
  if (WholeFile.find_last_of("/") != std::string::npos) {
    return WholeFile.substr(0, WholeFile.find_last_of("/"));
  }
  return ".";
}

std::string GetParentDirectoryName(const std::string& WholeFile) {
  std::string dir_name = GetFilename(GetDirectory(WholeFile));
  return dir_name;
}

bool CreateDirectory(const std::string& dir) {
  // From stackoverflow.com  "Recursive mkdir() system call on Unix"
  unsigned int mode = S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH;
  char tmp[1024];
  char* p = nullptr;
  size_t len;
  snprintf(tmp, sizeof(tmp), "%s", dir.c_str());
  len = dir.size();
  if (tmp[len - 1] == '/') tmp[len - 1] = 0;
  for (p = tmp + 1; *p; p++) {
    if (*p == '/') {
      *p = 0;
      mkdir(tmp, mode);
      *p = '/';
    }
  }
  mkdir(tmp, mode);

  return dir_exists(dir.c_str());
}

static char* fcntl_flags(int flags) {
  static char output[128];
  *output = 0;

  if (flags & O_RDONLY) strcat(output, "O_RDONLY ");
  if (flags & O_WRONLY) strcat(output, "O_WRONLY ");
  if (flags & O_RDWR) strcat(output, "O_RDWR ");
  if (flags & O_CREAT) strcat(output, "O_CREAT ");
  if (flags & O_EXCL) strcat(output, "O_EXCL ");
  if (flags & O_NOCTTY) strcat(output, "O_NOCTTY ");
  if (flags & O_TRUNC) strcat(output, "O_TRUNC ");
  if (flags & O_APPEND) strcat(output, "O_APPEND ");
  if (flags & O_NONBLOCK) strcat(output, "O_NONBLOCK ");
  if (flags & O_SYNC) strcat(output, "O_SYNC ");
  if (flags & O_ASYNC) strcat(output, "O_ASYNC ");

  return output;
}

static char* fd_info(int fd) {
  if (fd < 0 || fd >= FD_SETSIZE) return nullptr;
  // if (fcntl(fd, F_GETFL) == -1 && errno == EBADF)
  int rv = fcntl(fd, F_GETFL);
  return (rv == -1) ? strerror(errno) : fcntl_flags(rv);
}

/* check first 1024 (usual size of FD_SESIZE) file handles */
void test_fds() {
  int i;
  int fd_dup;
  char errst[64];
  for (i = 0; i < 20; i++) {
    *errst = 0;
    fd_dup = dup(i);
    if (fd_dup == -1) {
      strcpy(errst, strerror(errno));
      // EBADF  oldfd isnâ€™t an open file descriptor, or newfd is out of the allowed range for file
      // descriptors. EBUSY  (Linux only) This may be returned by dup2() during a race condition with open(2)
      // and dup(). EINTR  The dup2() call was interrupted by a signal; see signal(7). EMFILE The process
      // already has the maximum number of file descriptors open and tried to open a new one.
    } else {
      close(fd_dup);
      strcpy(errst, "dup() ok");
    }
    printf("%4i: %5i %24s %s\n", i, fcntl(i, F_GETOWN), fd_info(i), errst);
  }
}

bool IsAbsolutePath(const std::string_view filepath) { return fs::path{filepath}.is_absolute(); }

bool GetAbsolutePath(const std::string_view FilePath, std::string& AbsolutePath,
                     std::optional<std::reference_wrapper<std::error_code>> ErrorCode) {
  bool success = false;
  fs::path ExpandedPath;
  if (!FilePath.empty() && FilePath.at(0) == '~') {
    // expand '~' if necessary
    auto expanded_path_string = GetHomeFolder();
    const auto remaining_path = std::string_view(FilePath.data() + 1, FilePath.length() - 1);
    expanded_path_string.append(remaining_path);
    ExpandedPath = expanded_path_string;
  } else {
    ExpandedPath = FilePath;
  }

  std::error_code ec;
#if (defined(_GLIBCXX_RELEASE) && (_GLIBCXX_RELEASE > 7)) || \
    (defined(_LIBCPP_VERSION) && (_LIBCPP_VERSION > 10000))
  AbsolutePath = fs::canonical(ExpandedPath, ec).string();
#else
  auto BasePath = fs::current_path();
  AbsolutePath = fs::canonical(ExpandedPath, BasePath, ec).string();
#endif
  if (!ec) {
    success = true;
  } else {
    vlog_warning(VCAT_GENERAL, "Warning: '%s' for trying to compute Absolute path from path: '%s'",
                 ec.message().c_str(), ExpandedPath.c_str());
  }

  if (ErrorCode) {
    ErrorCode->get().assign(ec.value(), ec.category());
  }

  return success;
}

// Recursive function to perform wildcard matching between two null
// terminated strings. The only wildcard supported is '*'. This probably will
// not work properly if str has '*' present in it as this token is defined as
// wildcard
// Error Codes/Scenarios and edge cases:
//   This function does not handle escaping of wildcards. The behavior is undefined if
//   the str also has *
// Args:
//   wild: The pattern to match and contains wildcard
//   str: The string to check if it matches wild
bool Match(const char* wild, const char* str) {
  if (*wild == 0 && *str == 0) {
    return true;
  }
  if (*wild == '*') {
    if (Match(wild + 1, str)) return true;
    if (*str == 0) return false;
    return Match(wild, str + 1);
  }
  if (*wild == *str) {
    return Match(wild + 1, str + 1);
  }
  return false;
}

// Trim string from left
static inline void TrimStringLeft(std::string& str) {
  str.erase(str.begin(),
            std::find_if(str.begin(), str.end(), [](unsigned char ch) { return !std::isspace(ch); }));
}

// Trim string from right
static inline void TrimStringRight(std::string& str) {
  str.erase(std::find_if(str.rbegin(), str.rend(), [](unsigned char ch) { return !std::isspace(ch); }).base(),
            str.end());
}

// Trim both leading & trailing substring from string
void TrimString(std::string& str) {
  TrimStringLeft(str);
  TrimStringRight(str);
}

// Get the user name on Linux.  Useful for concatenating absolute paths.
std::string GetUserName() {
  uid_t uid = geteuid();
  struct passwd* pw = getpwuid(uid);  // This works inside vdev container
  if (pw) {
    // printf( "getpwuid: %s\n", pw->pw_name );
    return std::string(pw->pw_name);
  }
  return std::string();
}

std::string GetExecutablePath() {
#ifdef _WIN32
  wchar_t path[MAX_PATH] = {0};
  GetModuleFileNameW(NULL, path, MAX_PATH);
  return path;
#else
  char result[PATH_MAX];
  ssize_t count = readlink("/proc/self/exe", result, PATH_MAX);
  return std::string(result, (count > 0) ? count : 0);
#endif
}

namespace toolbox_internal {

fs::path PathConcatImpl(const std::string_view path) { return fs::path{path}; }

}  // namespace toolbox_internal

std::vector<std::string> FindFilesRecursive(const std::string& start_path,
                                            const std::string& file_extension) {
  std::vector<std::string> files;
  // If path does not exists, return empty vector and leave
  if (!dir_exists(start_path.c_str())) return files;
  for (const fs::directory_entry& dir_entry :
       fs::recursive_directory_iterator(start_path, fs::directory_options::skip_permission_denied)) {
    if (!file_extension.empty()) {
      std::string ext = GetFileExtension(dir_entry.path().string());
      if (ext == file_extension) files.emplace_back(dir_entry.path().string());
    } else {
      files.emplace_back(file_extension);
    }
  }
  return files;
}
