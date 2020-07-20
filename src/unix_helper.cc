#include "toolbox/unix_helper.h"

#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>

#include <array>
#include <iostream>
#include <memory>
#include <string>

#include "vlog.h"

bool exec(const char* cmd, std::string& output) {
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

char getchar_non_canonical(bool blocking) {
  char buf = 0;
  struct termios old {};
  if (tcgetattr(0, &old) < 0) {
    perror("tcsetattr()");
  }
  old.c_lflag &= ~ICANON;
  old.c_lflag &= ~ECHO;
  old.c_cc[VMIN] = 1;
  if (!blocking) {
    old.c_cc[VMIN] = 0;
  }
  old.c_cc[VTIME] = 0;
  if (tcsetattr(0, TCSANOW, &old) < 0) {
    perror("tcsetattr ICANON");
  }
  if (read(0, &buf, 1) < 0) {
    perror("read()");
  }
  old.c_lflag |= ICANON;
  old.c_lflag |= ECHO;
  if (tcsetattr(0, TCSADRAIN, &old) < 0) {
    perror("tcsetattr ~ICANON");
  }
  return (buf);
}
