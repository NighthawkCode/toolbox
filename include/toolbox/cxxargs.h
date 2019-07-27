#pragma once

#include "cxxopts.hpp"

/*
 * Helper functions to make using cxx easier
 * and avoid the whole try catch
 */

bool get_arg_int(cxxopts::ParseResult& parse, const std::string& name, int &val)
{
  try {
    val = parse[name].as<int>();
    return true;
  } catch (...) {
    return false;
  }
}

bool get_arg_bool(cxxopts::ParseResult& parse, const std::string& name, bool &val)
{
  try {
    val = parse[name].as<bool>();
    return true;
  } catch (...) {
    return false;
  }
}

bool get_arg_string(cxxopts::ParseResult& parse, const std::string& name, std::string &val)
{
  try {
    val = parse[name].as<std::string>();
    return true;
  } catch (...) {
    return false;
  }
}
