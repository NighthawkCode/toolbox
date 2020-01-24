#pragma once
#include <string>
#include <vector>

std::string ReadFileIntoString( const char *filename );
bool ReadFileIntoString( const char *filename, std::string &contents );
bool file_exists( const char* filepath );
bool dir_exists( const char* dirpath );
bool TouchFile(const char *filepath);
std::string GetHomeFolder();
std::vector<std::string> GetFilesInDirectory(const std::string& dir);
std::string GetFileExtension(const std::string& FileName);
std::string GetFilename(const std::string& WholeFile);
std::string GetDirectory(const std::string& WholeFile);

// Create directory if it doesn't already exist.  Return bool indicating
// existence of directory.
bool MaybeCreateDirectory(const std::string& dir);

void test_fds();
