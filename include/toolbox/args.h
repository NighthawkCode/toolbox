#pragma once

#include <cxxabi.h>
#include <vlog.h>

#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include "split.h"

template <typename type>
std::string TypeToString() {
  char* name = abi::__cxa_demangle(typeid(type).name(), 0, 0, NULL);
  std::string r(name);
  free(name);
  return r;
}

// Base class for options
class ArgOption {
public:
  std::vector<std::string> names;
  std::string expl;
  bool seen = false;

  ArgOption(const std::string& name, const std::string& explanation) {
    names.push_back(name);
    expl = explanation;
  }

  ArgOption(const std::vector<std::string> names_, const std::string& explanation) {
    names = names_;
    expl = explanation;
  }

  bool match(char* argument) {
    for (auto& n : names) {
      if (n == argument) {
        return true;
      }
    }
    return false;
  }

  virtual bool consumeArgs(int& index, int argc, char** argv) = 0;
  virtual ~ArgOption() {}
};

class ArgOptionBool : public ArgOption {
public:
  bool default_val;
  bool& output_val;

  ArgOptionBool(const std::string& name, const std::string& explanation, bool& output_, bool def_val = false)
      : ArgOption(name, explanation)
      , default_val(def_val)
      , output_val(output_) {
    output_val = default_val;
  }

  ArgOptionBool(const std::vector<std::string> names_, const std::string& explanation, bool& output_,
                bool def_val = false)
      : ArgOption(names_, explanation)
      , default_val(def_val)
      , output_val(output_) {
    output_val = default_val;
  }

  bool consumeArgs(int& index, int argc, char** argv) override {
    (void)index;
    (void)argc;
    (void)argv;
    output_val = true;
    seen = true;
    return true;
  }
};
template <typename T>
class ArgOptionNumber : public ArgOption {
public:
  T default_val;
  T& output_val;
  ArgOptionNumber(const std::string& name, const std::string& explanation, T& output_, T def_val = -1)
      : ArgOption(name, explanation)
      , default_val(def_val)
      , output_val(output_) {
    output_val = default_val;
  }

  ArgOptionNumber(const std::vector<std::string> names_, const std::string& explanation, T& output_,
                  T def_val = -1)
      : ArgOption(names_, explanation)
      , default_val(def_val)
      , output_val(output_) {
    output_val = default_val;
  }

  bool consumeArgs(int& index, int argc, char** argv) override {
    if (index >= argc) {
      vlog_error(VCAT_GENERAL, "Argument \"%s\" needs a parameter", names.front().c_str());
      return false;
    }
    std::istringstream ss(argv[index++]);
    ss >> output_val;
    if (ss.fail()) {
      vlog_error(VCAT_GENERAL, "Argument \"%s\" was unable to convert \"%s\" to %s", names.front().c_str(),
                 argv[index - 1], TypeToString<T>().c_str());
      return false;
    }
    seen = true;
    return true;
  }
};

using ArgOptionInt = ArgOptionNumber<int>;
using ArgOptionDouble = ArgOptionNumber<double>;

class ArgOptionFloat : public ArgOption {
public:
  float default_val;
  float& output_val;
  ArgOptionFloat(const std::string& name, const std::string& explanation, float& output_, float def_val)
      : ArgOption(name, explanation)
      , default_val(def_val)
      , output_val(output_) {
    output_val = default_val;
  }

  ArgOptionFloat(const std::vector<std::string> names_, const std::string& explanation, float& output_,
                 float def_val)
      : ArgOption(names_, explanation)
      , default_val(def_val)
      , output_val(output_) {
    output_val = default_val;
  }

  ArgOptionFloat(const std::string& name, const std::string& explanation, float& output_)
      : ArgOption(name, explanation)
      , output_val(output_) {}

  ArgOptionFloat(const std::vector<std::string> names_, const std::string& explanation, float& output_)
      : ArgOption(names_, explanation)
      , output_val(output_) {}

  bool consumeArgs(int& index, int argc, char** argv) override {
    if (index >= argc) {
      vlog_error(VCAT_GENERAL, "Argument \"%s\" needs a parameter", names.front().c_str());
      return false;
    }
    output_val = atof(argv[index++]);
    seen = true;
    return true;
  }
};

class ArgOptionString : public ArgOption {
public:
  std::string default_val;
  std::string& output_val;
  ArgOptionString(const std::string& name, const std::string& explanation, std::string& output_,
                  std::string def_val = "")
      : ArgOption(name, explanation)
      , default_val(def_val)
      , output_val(output_) {
    output_val = default_val;
  }

  ArgOptionString(const std::vector<std::string> names_, const std::string& explanation, std::string& output_,
                  std::string def_val = "")
      : ArgOption(names_, explanation)
      , default_val(def_val)
      , output_val(output_) {
    output_val = default_val;
  }

  bool consumeArgs(int& index, int argc, char** argv) override {
    if (index >= argc) {
      vlog_error(VCAT_GENERAL, "Argument \"%s\" needs a parameter", names.front().c_str());
      return false;
    }
    output_val = argv[index++];
    seen = true;
    return true;
  }
};

// Supports comma-separated list of integers, including ranges, e.g.:
// --arg 10         # 10
// --arg 1,2,3      # 1, 2, 3
// --arg 1:3        # 1, 2, 3
// --arg 1,3:8,10   # 1, 3, 4, 5, 6, 7, 8, 10
// --arg -5:-2      # -5, -4, -3, -2
class ArgOptionIntList : public ArgOption {
public:
  std::vector<int>& output_val;
  ArgOptionIntList(const std::string& name, const std::string& explanation, std::vector<int>& output_)
      : ArgOption(name, explanation)
      , output_val(output_) {}

  ArgOptionIntList(const std::vector<std::string> names_, const std::string& explanation,
                   std::vector<int>& output_)
      : ArgOption(names_, explanation)
      , output_val(output_) {}

  bool consumeArgs(int& index, int argc, char** argv) override {
    if (index >= argc) {
      vlog_error(VCAT_GENERAL, "Argument \"%s\" needs a parameter", names.front().c_str());
      return false;
    }
    if (toolbox::split_int_range(argv[index++], output_val)) {
      seen = true;
      return true;
    }
    vlog_error(VCAT_GENERAL, "Failed to parse IntList argument %s: %s", names.front().c_str(),
               argv[index - 1]);
    return false;
  }
};

class ArgOptionStringList : public ArgOption {
public:
  std::vector<std::string>& output_val;
  ArgOptionStringList(const std::string& name, const std::string& explanation,
                      std::vector<std::string>& output_)
      : ArgOption(name, explanation)
      , output_val(output_) {}

  ArgOptionStringList(const std::vector<std::string> names_, const std::string& explanation,
                      std::vector<std::string>& output_)
      : ArgOption(names_, explanation)
      , output_val(output_) {}

  bool consumeArgs(int& index, int argc, char** argv) override {
    if (index >= argc) {
      vlog_error(VCAT_GENERAL, "Argument \"%s\" needs a parameter", names.front().c_str());
      return false;
    }
    auto split_str = toolbox::split(argv[index++], ',');
    for (auto& s : split_str) {
      output_val.push_back(s);
    }
    seen = true;
    return true;
  }
};

class ArgOptionFloatList : public ArgOption {
public:
  std::vector<float>& output_val;
  ArgOptionFloatList(const std::string& name, const std::string& explanation, std::vector<float>& output_)
      : ArgOption(name, explanation)
      , output_val(output_) {}

  ArgOptionFloatList(const std::vector<std::string> names_, const std::string& explanation,
                     std::vector<float>& output_)
      : ArgOption(names_, explanation)
      , output_val(output_) {}

  bool consumeArgs(int& index, int argc, char** argv) override {
    if (index >= argc) {
      vlog_error(VCAT_GENERAL, "Argument \"%s\" needs a parameter", names.front().c_str());
      return false;
    }
    auto split_str = toolbox::split(argv[index++], ',');
    for (auto& s : split_str) {
      output_val.push_back(std::stof(s));
    }
    seen = true;
    return true;
  }
};

class ArgParser {
  std::vector<std::shared_ptr<ArgOption>> options;
  std::string program_name;
  std::string program_description;

  std::string list_options(const std::vector<std::string>& names) {
    std::string res;
    for (size_t i = 0; i < names.size(); i++) {
      if (i > 0) res += ", ";
      res += names[i];
    }
    return res;
  }

  bool allow_positional = false;
  bool allow_multiple_positional = false;
  std::string positional_name;
  std::string positional_expl;
  std::vector<std::string> positional;

public:
  ArgParser(const std::string& pname, const std::string& pdesc)
      : program_name(pname)
      , program_description(pdesc) {}

  template <typename T>
  ArgParser& AddOption(T opt) {
    options.push_back(std::make_shared<T>(opt));
    return *this;
  }

  void SetPositional(bool allow, bool multiple, const std::string& name, const std::string& expl) {
    allow_positional = allow;
    allow_multiple_positional = multiple;
    positional_name = name;
    positional_expl = expl;
  }

  const std::vector<std::string>& Positional() const { return positional; }

  bool ParseArgs(int argc, char** argv) {
    int index;
    for (index = 1; index < argc;) {
      std::shared_ptr<ArgOption> opt;
      for (auto& o : options) {
        if (o->match(argv[index])) {
          opt = o;
          break;
        }
      }
      if (!opt) {
        // Positional arguments do not start with a dash
        if (allow_positional && (argv[index][0] != '-')) {
          break;
        }
        vlog_error(VCAT_GENERAL, "Argument \"%s\" could not be parsed", argv[index]);
        return false;
      }
      index++;
      if (!opt->consumeArgs(index, argc, argv)) {
        return false;
      }
    }
    if (index < argc) {
      positional.emplace_back(argv[index]);
      index++;
      if (index < argc && !allow_multiple_positional) {
        vlog_warning(VCAT_GENERAL, "Argument \"%s\" cannot be parsed", argv[index]);
      }
      for (; index < argc; index++) {
        positional.emplace_back(argv[index]);
      }
    }
    return true;
  }

  void PrintUsage() {
    printf("%s (c) Verdant Robotics - %s\n", program_name.c_str(), program_description.c_str());
    if (options.size()) {
      printf(" Usage: %s [OPTIONS]", program_name.c_str());
      if (allow_positional) {
        printf(" [%s]", positional_name.c_str());
      }
      printf("\n");
      for (auto& o : options) {
        printf("  %s : %s\n", list_options(o->names).c_str(), o->expl.c_str());
      }
      if (allow_positional) {
        printf("  %s : Must be at the end of the command line. %s\n", positional_name.c_str(),
               positional_expl.c_str());
      }
    }
  }
};

/*
 * ArgOptionBool("-h", "Display help", args.help);
 *
 *
 *
 */
