#include <stdio.h>
#include <stdlib.h>
#include "file_utils.h"

std::string read_file_into_string( const char *filename )
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
