#pragma once 
#include <dirent.h>
#include <string>
#include <regex>
#include <sys/stat.h>
#include <string>
#include <vector>

// Get a list of files from a directory, using regular expression to match.
// Example usage: auto strvec = dir_list( "/home/user/", ".*Token.*" ); 
inline 
std::vector<std::string> dir_list( const std::string& input_dir, const std::string& regexp )
{
  std::string dir = input_dir;
  if ( dir.back() == '/' ) {
    dir.pop_back(); // nuke trailing slashes
  }
  std::list<std::string> matched_files;
  struct dirent** namelist;
  int n = scandir( dir.c_str(), &namelist, 0, alphasort );
  if ( n >= 0 ) {
    std::list<std::string> files;
    while ( n-- ) {
      const std::string fname( namelist[n]->d_name );
      std::regex image_regex( regexp );
      if ( std::regex_match( fname, image_regex ) ) {
        matched_files.push_front( dir + "/" + fname );
      }
      free( namelist[n] );
    }
    free( namelist );
  }

  // convert to vector
  std::vector<std::string> file_vector;
  file_vector.reserve( matched_files.size() );
  file_vector.insert( file_vector.begin(), matched_files.begin(), matched_files.end() );
  return file_vector;
}
