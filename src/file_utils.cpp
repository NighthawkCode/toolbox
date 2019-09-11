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

std::string ReadFileIntoString( const char *filename )
{
  FILE* f;
  f = fopen(filename, "r");
  if (f == nullptr) {
    fprintf(stderr, "Could not open file %s for reading\n", filename);
    return std::string();
  }
  fseek(f, 0, SEEK_END);
  auto size = ftello64(f);
  std::string buf;
  buf.resize(size+1);
  buf[size] = 0;
  fseek(f, 0, SEEK_SET);
  auto bytes_read = fread(&buf[0], 1, size, f);
  if (bytes_read != size) {
    fprintf(stderr, "Failed to read required bytes from file %s\n", filename);
    buf.resize(0);
  }
  fclose(f);
  return buf;
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

bool MaybeCreateDirectory(const std::string& dir) {
  if( !std::experimental::filesystem::exists( dir.c_str() ) ) {
    if( !std::experimental::filesystem::create_directories( dir.c_str() ) ) {
      return false;
    }
  }
  return true;
}
