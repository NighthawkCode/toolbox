#pragma once
#include <string>
#include <vector>

std::string ReadFileIntoString( const char *filename );
bool file_exists( const char* filepath );
bool dir_exists( const char* dirpath );
std::string GetHomeFolder();
std::vector<std::string> GetFilesInDirectory(const std::string& dir);
std::string GetFileExtension(const std::string& FileName);
std::string GetFilename(const std::string& WholeFile);
