#pragma once
#include "split.h"
#include <memory>
#include <string>
#include <vector>

// Base class for options
class ArgOption {
public:
  std::vector<std::string> names;
  std::string expl;
  bool seen = false;

  ArgOption(const std::string &name, const std::string &explanation) {
    names.push_back(name);
    expl = explanation;
  }

  ArgOption(const std::vector<std::string> names_,
            const std::string &explanation) {
    names = names_;
    expl = explanation;
  }

  bool match(char *argument) {
    for (auto &n : names) {
      if (n == argument) {
        return true;
      }
    }
    return false;
  }

  virtual bool consumeArgs(int &index, int argc, char **argv) = 0;
  virtual ~ArgOption(){};
};

class ArgOptionBool : public ArgOption {
public:
  bool default_val;
  bool &output_val;

  ArgOptionBool(const std::string &name, const std::string &explanation,
                bool &output_, bool def_val = false)
      : ArgOption(name, explanation), default_val(def_val),
        output_val(output_) {
    output_val = default_val;
  }

  ArgOptionBool(const std::vector<std::string> names_,
                const std::string &explanation, bool &output_,
                bool def_val = false)
      : ArgOption(names_, explanation), default_val(def_val),
        output_val(output_) {
    output_val = default_val;
  }

  bool consumeArgs(int &index, int argc, char **argv) {
    output_val = true;
    seen = true;
    return true;
  }
};

class ArgOptionInt : public ArgOption {
public:
  int default_val;
  int &output_val;
  ArgOptionInt(const std::string &name, const std::string &explanation,
               int &output_, int def_val = -1)
      : ArgOption(name, explanation), default_val(def_val),
        output_val(output_) {
    output_val = default_val;
  }

  ArgOptionInt(const std::vector<std::string> names_,
               const std::string &explanation, int &output_, int def_val = -1)
      : ArgOption(names_, explanation), default_val(def_val),
        output_val(output_) {
    output_val = default_val;
  }

  bool consumeArgs(int &index, int argc, char **argv) {
    if (index >= argc) {
      printf("Error, argument %s needs a parameter\n", names.front().c_str());
      return false;
    }
    output_val = atoi(argv[index++]);
    seen = true;
    return true;
  }
};

class ArgOptionFloat : public ArgOption {
public:
  float default_val;
  float &output_val;
  ArgOptionFloat(const std::string &name, const std::string &explanation,
                 float &output_, float def_val = -1)
      : ArgOption(name, explanation), default_val(def_val),
        output_val(output_) {
    output_val = default_val;
  }

  ArgOptionFloat(const std::vector<std::string> names_,
                 const std::string &explanation, float &output_,
                 float def_val = -1)
      : ArgOption(names_, explanation), default_val(def_val),
        output_val(output_) {
    output_val = default_val;
  }

  bool consumeArgs(int &index, int argc, char **argv) {
    if (index >= argc) {
      printf("Error, argument %s needs a parameter\n", names.front().c_str());
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
  std::string &output_val;
  ArgOptionString(const std::string &name, const std::string &explanation,
                  std::string &output_, std::string def_val = "")
      : ArgOption(name, explanation), default_val(def_val),
        output_val(output_) {
    output_val = default_val;
  }

  ArgOptionString(const std::vector<std::string> names_,
                  const std::string &explanation, std::string &output_,
                  std::string def_val = "")
      : ArgOption(names_, explanation), default_val(def_val),
        output_val(output_) {
    output_val = default_val;
  }

  bool consumeArgs(int &index, int argc, char **argv) {
    if (index >= argc) {
      printf("Error, argument %s needs a parameter\n", names.front().c_str());
      return false;
    }
    output_val = argv[index++];
    seen = true;
    return true;
  }
};

class ArgOptionIntList : public ArgOption {
public:
  std::vector<int> &output_val;
  ArgOptionIntList(const std::string &name, const std::string &explanation,
                   std::vector<int> &output_)
      : ArgOption(name, explanation), output_val(output_) {}

  ArgOptionIntList(const std::vector<std::string> names_,
                   const std::string &explanation, std::vector<int> &output_)
      : ArgOption(names_, explanation), output_val(output_) {}

  bool consumeArgs(int &index, int argc, char **argv) {
    if (index >= argc) {
      printf("Error, argument %s needs a parameter\n", names.front().c_str());
      return false;
    }
    auto split_str = split(argv[index++], ',');
    for (auto &s : split_str) {
      output_val.push_back(atoi(s.c_str()));
    }
    seen = true;
    return true;
  }
};

class ArgOptionStringList : public ArgOption {
public:
  std::vector<std::string> &output_val;
  ArgOptionStringList(const std::string &name, const std::string &explanation,
                      std::vector<std::string> &output_)
      : ArgOption(name, explanation), output_val(output_) {}

  ArgOptionStringList(const std::vector<std::string> names_,
                      const std::string &explanation,
                      std::vector<std::string> &output_)
      : ArgOption(names_, explanation), output_val(output_) {}

  bool consumeArgs(int &index, int argc, char **argv) {
    if (index >= argc) {
      printf("Error, argument %s needs a parameter\n", names.front().c_str());
      return false;
    }
    auto split_str = split(argv[index++], ',');
    for (auto &s : split_str) {
      output_val.push_back(s);
    }
    seen = true;
    return true;
  }
};

class ArgParser {
  std::vector<std::shared_ptr<ArgOption>> options;
  std::string program_name;
  std::string program_description;

  std::string list_options(const std::vector<std::string> &names) {
    std::string res;
    for (int i = 0; i < names.size(); i++) {
      if (i > 0)
        res += ",";
      res += names[i];
    }
    return res;
  }

public:
  ArgParser(const std::string &pname, const std::string &pdesc)
      : program_name(pname), program_description(pdesc) {}

  template <typename T> ArgParser &AddOption(T opt) {
    options.push_back(std::make_shared<T>(opt));
    return *this;
  }

  bool ParseArgs(int argc, char **argv) {
    for (int index = 1; index < argc;) {
      std::shared_ptr<ArgOption> opt;
      for (auto &o : options) {
        if (o->match(argv[index])) {
          opt = o;
          break;
        }
      }
      if (!opt) {
        printf(" Error, argument %s could not be parsed", argv[index]);
        return false;
      }
      index++;
      if (!opt->consumeArgs(index, argc, argv)) {
        return false;
      }
    }
    return true;
  }

  void PrintUsage() {
    printf("%s (c) Verdant Robotics - %s\n", program_name.c_str(),
           program_description.c_str());
    if (options.size()) {
      printf(" Usage: %s [OPTIONS]\n", program_name.c_str());
      for (auto &o : options) {
        printf("  %s : %s\n", list_options(o->names).c_str(), o->expl.c_str());
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
