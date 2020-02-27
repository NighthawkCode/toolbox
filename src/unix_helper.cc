#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <iostream>
#include <memory>
#include <string>
#include <array>
#include "vlog.h"

#include "toolbox/unix_helper.h"

bool exec(const char* cmd, std::string &output) {
  output = "";
  std::array<char, 128> buffer;
  std::string result;
  std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
  if (!pipe) {
    vlog_warning(VCAT_GENERAL, "exec: popen() failed!");
    return false;
  }
  while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
    result += buffer.data();
  }

  // remove newline characters from result
  while (result.find("\n") != std::string::npos) {
    result.erase(result.find("\n"), 1);
  }

  return false;
}

