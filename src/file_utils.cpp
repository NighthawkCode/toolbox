#include <stdio.h>
#include <stdlib.h>
#include "file_utils.h"
#include <sys/statvfs.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include <dirent.h>
#include <experimental/filesystem> //#include <filesystem>
#include <errno.h>
#include <string.h>
#include <sys/resource.h>
#include <dirent.h>
#include <fcntl.h>
#include <fstream>
#include <streambuf>

#include "vlog.h"

std::string ReadFileIntoString( const std::string_view filename ) {
  std::string contents;
  ReadFileIntoString(filename, contents);
  return contents;
}

bool ReadFileIntoString( const std::string_view filename, std::string &buf ) {
  std::ifstream t(filename.data());
  if (!t.is_open()) {
    vlog_error(VCAT_GENERAL, "Could not open file %s for reading", filename.data());
    return false;
  }

  t.seekg(0, std::ios::end);
  buf.reserve(t.tellg());
  t.seekg(0, std::ios::beg);
  buf.assign((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
  return t.good();
}

bool file_exists( const char* filepath )
{
  struct stat status;
  if( stat( filepath, &status ) == 0 && S_ISREG( status.st_mode ) ) {
    return true;
  }
  return false;
}

bool dir_exists( const char* dirpath )
{
  struct stat status;
  if( stat( dirpath, &status ) == 0 && S_ISDIR( status.st_mode ) ) {
    return true;
  }
  return false;
}

bool TouchFile(const char *filepath)
{
  int fd = open(filepath, O_WRONLY|O_CREAT|O_NOCTTY|O_NONBLOCK, 0666);
  if (fd < 0) {
    return false;
  }
  close(fd);
  return true;
}

std::string GetHomeFolder()
{
  const char *home_dir = nullptr;
  home_dir = getenv("HOME");
  if (home_dir == nullptr) {
    home_dir = getpwuid(getuid())->pw_dir;
  }
  return std::string(home_dir);
}

std::vector<std::string> GetFilesInDirectory(const std::string& dir)
{
  std::vector<std::string> vec;

  DIR *dirp = opendir(dir.c_str());
  if (dirp == nullptr) return vec;

  struct dirent *dp = readdir(dirp);

  while(dp != nullptr) {
    vec.push_back(std::string(dp->d_name));
    dp = readdir(dirp);
  }
  closedir(dirp);

  return vec;
}

std::string GetFileExtension(const std::string& FileName)
{
    if(FileName.find_last_of(".") != std::string::npos)
        return FileName.substr(FileName.find_last_of(".")+1);
    return "";
}

std::string GetFilename(const std::string& WholeFile)
{
    if(WholeFile.find_last_of("/") != std::string::npos)
        return WholeFile.substr(WholeFile.find_last_of("/")+1);
    return WholeFile;
}

std::string GetDirectory(const std::string& WholeFile)
{
  if(WholeFile.find_last_of("/") != std::string::npos) {
    return WholeFile.substr(0, WholeFile.find_last_of("/"));
  }
  return ".";
}

bool CreateDirectory(const std::string& dir) {
  // From stackoverflow.com  "Recursive mkdir() system call on Unix"
  unsigned int mode = S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH;
  char tmp[1024];
  char *p = nullptr;
  size_t len;
  snprintf(tmp, sizeof(tmp),"%s",dir.c_str());
  len = dir.size();
  if(tmp[len - 1] == '/')
      tmp[len - 1] = 0;
  for(p = tmp + 1; *p; p++) {
    if(*p == '/') {
      *p = 0;
      mkdir(tmp, mode);
      *p = '/';
    }
  }
  mkdir(tmp, mode);

  return dir_exists(dir.c_str());
}

static char *fcntl_flags(int flags)
{
    static char output[128];
    *output = 0;

    if (flags & O_RDONLY)
        strcat(output, "O_RDONLY ");
    if (flags & O_WRONLY)
        strcat(output, "O_WRONLY ");
    if (flags & O_RDWR)
        strcat(output, "O_RDWR ");
    if (flags & O_CREAT)
        strcat(output, "O_CREAT ");
    if (flags & O_EXCL)
        strcat(output, "O_EXCL ");
    if (flags & O_NOCTTY)
        strcat(output, "O_NOCTTY ");
    if (flags & O_TRUNC)
        strcat(output, "O_TRUNC ");
    if (flags & O_APPEND)
        strcat(output, "O_APPEND ");
    if (flags & O_NONBLOCK)
        strcat(output, "O_NONBLOCK ");
    if (flags & O_SYNC)
        strcat(output, "O_SYNC ");
    if (flags & O_ASYNC)
        strcat(output, "O_ASYNC ");

    return output;
}

static char *fd_info(int fd)
{
    if (fd < 0 || fd >= FD_SETSIZE)
        return nullptr;
    // if (fcntl(fd, F_GETFL) == -1 && errno == EBADF)
    int rv = fcntl(fd, F_GETFL);
    return (rv == -1) ? strerror(errno) : fcntl_flags(rv);
}

/* check first 1024 (usual size of FD_SESIZE) file handles */
void test_fds()
{
     int i;
     int fd_dup;
     char errst[64];
     for (i = 0; i < 20; i++) {
          *errst = 0;
          fd_dup = dup(i);
          if (fd_dup == -1) {
                strcpy(errst, strerror(errno));
                // EBADF  oldfd isnâ€™t an open file descriptor, or newfd is out of the allowed range for file descriptors.
                // EBUSY  (Linux only) This may be returned by dup2() during a race condition with open(2) and dup().
                // EINTR  The dup2() call was interrupted by a signal; see signal(7).
                // EMFILE The process already has the maximum number of file descriptors open and tried to open a new one.
          } else {
                close(fd_dup);
                strcpy(errst, "dup() ok");
          }
          printf("%4i: %5i %24s %s\n", i, fcntl(i, F_GETOWN), fd_info(i), errst);
     }
}
