#pragma once
#include <string>

std::string ReadFileIntoString( const char *filename );
bool file_exists( const char* filepath );
bool dir_exists( const char* dirpath );
std::string GetHomeFolder();
