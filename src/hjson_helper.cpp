#include "hjson_helper.h"

#include <errno.h>
#include <math.h>
#include <string.h>

#include <fstream>
#include <string_view>
#include <unordered_map>

#include "file_utils.h"
#include "types.h"
#include "vlog.h"

#if (defined(_GLIBCXX_RELEASE) && (_GLIBCXX_RELEASE > 7)) || \
    (defined(_LIBCPP_VERSION) && (_LIBCPP_VERSION > 10000))
#include <filesystem>
namespace fs = std::filesystem;
#else
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;
#endif

static constexpr const char* dots = "...";

static bool load_json_file(Hjson::Value& json, const std::string_view filename,
                           std::unordered_map<std::string, int> included_files);

// Look for any expandable strings
static bool find_dots(Hjson::Value& json) {
  if (json.type() == Hjson::Type::Map) {
    if (json[dots].defined() && json[dots].type() == Hjson::Type::String) {
      return true;
    }
    for (auto& v : json) {
      if (find_dots(v.second)) return true;
    }
  } else if (json.type() == Hjson::Type::Vector) {
    for (size_t i = 0; i < json.size(); i++) {
      if (find_dots(json[int(i)])) return true;
    }
  }
  return false;
}

static bool expand_json(Hjson::Value& json, const fs::path& parentpath,
                        const std::unordered_map<std::string, int>& included_files) {
  if (json.type() == Hjson::Type::Map) {
    if (json[dots].defined() && json[dots].type() == Hjson::Type::String) {
      // Load the new json file here, replace and delete
      Hjson::Value loaded_json;
      fs::path path_to_load = parentpath / fs::path(json[dots].to_string());
      if (!load_json_file(loaded_json, path_to_load.c_str(), included_files)) return false;
      json.erase(dots);
      merge_json(json, loaded_json, false);
    }
    for (auto& v : json) {
      if (!expand_json(v.second, parentpath, included_files)) return false;
    }
  } else if (json.type() == Hjson::Type::Vector) {
    for (size_t i = 0; i < json.size(); i++) {
      if (!expand_json(json[int(i)], parentpath, included_files)) return false;
    }
  }
  return true;
}

// This is a recursive function
static bool load_json_file(Hjson::Value& json, const std::string_view filename,
                           std::unordered_map<std::string, int> included_files) {
  std::string text;
  if (!ReadFileIntoString(filename, text)) return false;
  try {
    json = Hjson::Unmarshal(text.c_str());
  } catch (const std::exception& ex) {
    vlog_error(VCAT_GENERAL, "Could not parse json file, EXCEPTION %s:\n%s", ex.what(), text.c_str());
    return false;
  } catch (...) {
    vlog_error(VCAT_GENERAL, "Could not parse json file:\n%s", text.c_str());
    return false;
  }

  fs::path fpath = filename;
  fs::path fabspath = fs::absolute(fpath);
  fs::path fparentpath = fabspath.parent_path();
  if (included_files.count(fpath.c_str())) {
    vlog_error(VCAT_GENERAL, "Error, circular inclusion of %s!", fpath.c_str());
    return false;
  }

  included_files[fpath.c_str()] = 1;

  if (!expand_json(json, fparentpath, included_files)) {
    return false;
  }

  return true;
}

bool load_json(Hjson::Value& json, const std::string_view text) {
  try {
    json = Hjson::Unmarshal(text.data(), text.size());
    if (find_dots(json)) {
      vlog_warning(VCAT_GENERAL,
                   "Warning, while loading a json from a string we found an include that cannot be "
                   "expanded.\nJSON: %.*s",
                   static_cast<int>(text.length()), text.data());
    }
    return true;
  } catch (...) {
    vlog_error(VCAT_GENERAL, "Could not parse json configuration file %.*s", static_cast<int>(text.length()),
               text.data());
    return false;
  }
}

bool load_json_file(Hjson::Value& json, const std::string_view filename) {
  std::unordered_map<std::string, int> incfiles;
  return load_json_file(json, filename, incfiles);
}

bool save_json(const Hjson::Value& json, const std::string_view filename, bool no_space, bool append) {
  try {
    auto opt = Hjson::EncoderOptions();
    opt.quoteKeys = true;
    opt.separator = true;
    if (no_space) {
      opt.eol = "";
      opt.indentBy = "";
    } else {
      opt.indentBy = "  ";
    }
    std::string text = Hjson::Marshal(json, opt);
    std::ofstream file;
    if (append)
      file.open(filename.data(), std::ios_base::app);
    else
      file.open(filename.data(), std::ios_base::trunc);

    if (!file.is_open()) {
      vlog_error(VCAT_GENERAL, "Failed to open %s for writing: %s", filename.data(), strerror(errno));
      return false;
    }

    if (append)
      file << text + "\n";
    else
      file << text;
    file.close();
    return true;
  } catch (const std::exception& e) {
    vlog_error(VCAT_GENERAL, "Exception error trying to save json %s: %s", filename.data(), e.what());
    return false;
  } catch (...) {
    vlog_error(VCAT_GENERAL, "Could not Marshal json to file %s", filename.data());
    return false;
  }
}

void merge_json(Hjson::Value& base, const Hjson::Value& add, bool overwrite) {
  if (base.type() != Hjson::Type::Map || add.type() != Hjson::Type::Map) {
    return;
  }
  for (auto it = add.begin(); it != add.end(); it++) {
    if (!overwrite && base[it->first].defined()) continue;
    auto nv = it->second.clone();
    base[it->first] = nv;
  }
}

bool has_member(const Hjson::Value& doc, const std::string& objName) {
  auto v = doc[objName];
  if (!v.defined()) {
    return false;
  }
  return true;
}

bool check_property_string(const char* parser, const Hjson::Value& o, const char* prop) {
  if (!has_member(o, prop)) {
    vlog_error(VCAT_GENERAL, "%s configuration needs to have a '%s' property\n", parser, prop);
    return false;
  }
  if (o[prop].type() != Hjson::Type::String) {
    vlog_error(VCAT_GENERAL, "%s configuration, '%s' needs to be a string\n", parser, prop);
    return false;
  }
  return true;
}

bool check_property_bool(const char* parser, const Hjson::Value& o, const char* prop) {
  if (!has_member(o, prop)) {
    vlog_error(VCAT_GENERAL, "%s configuration file needs to have a '%s' property\n", parser, prop);
    return false;
  }
  if (o[prop].type() != Hjson::Type::Bool) {
    vlog_error(VCAT_GENERAL, "%s configuration file, '%s' needs to be a Bool\n", parser, prop);
    return false;
  }
  return true;
}

bool check_property_obj(const char* parser, const Hjson::Value& o, const char* prop) {
  if (!has_member(o, prop)) {
    vlog_error(VCAT_GENERAL, "%s configuration file needs to have a '%s' property\n", parser, prop);
    return false;
  }
  if (o[prop].type() != Hjson::Type::Map) {
    vlog_error(VCAT_GENERAL, "%s configuration file, '%s' needs to be an object\n", parser, prop);
    return false;
  }
  return true;
}

void get_property_int(const Hjson::Value& o, int& val) {
  if (o.type() == Hjson::Type::String) {
    std::string v = o;
    val = std::atoi(o);
  } else if (o.type() == Hjson::Type::Double) {
    val = int(o);
  }
}

void get_property_float(const Hjson::Value& o, float& val) {
  if (o.type() == Hjson::Type::String) {
    std::string v = o;
    val = float(std::atof(o));
  } else if (o.type() == Hjson::Type::Double) {
    val = float(o);
  }
}

void get_property_bool(const Hjson::Value& o, bool& val, bool default_value) {
  if (!o.defined()) {
    val = default_value;
    return;
  }
  if (o.type() == Hjson::Type::String) {
    val = (o == std::string("true"));
  } else if (o.type() == Hjson::Type::Bool) {
    val = o.operator bool();
  } else if (o.type() == Hjson::Type::Double) {
    int v = int(o);
    val = bool(v);
  }
}

bool get_member_int(const Hjson::Value& doc, const std::string& objName, int8_t& val) {
  auto o = doc[objName];
  if (!o.defined()) {
    return false;
  }

  if (o.type() == Hjson::Type::String) {
    val = int8_t(std::atoi(o));
    return true;
  } else if (o.is_numeric()) {
    val = int8_t(o.operator char());
    return true;
  }
  return false;
}

bool get_member_int(const Hjson::Value& doc, const std::string& objName, int16_t& val) {
  auto o = doc[objName];
  if (!o.defined()) {
    return false;
  }

  if (o.type() == Hjson::Type::String) {
    val = int16_t(std::atoi(o));
    return true;
  } else if (o.is_numeric()) {
    val = int16_t(o);
    return true;
  }
  return false;
}

bool get_member_int(const Hjson::Value& doc, const std::string& objName, int32_t& val) {
  auto o = doc[objName];
  if (!o.defined()) {
    return false;
  }

  if (o.type() == Hjson::Type::String) {
    val = std::atoi(o);
    return true;
  } else if (o.is_numeric()) {
    val = int32_t(o);
    return true;
  }
  return false;
}

bool get_member_int(const Hjson::Value& doc, const std::string& objName, int64_t& val) {
  auto o = doc[objName];
  if (!o.defined()) {
    return false;
  }

  if (o.type() == Hjson::Type::String) {
    val = std::atoll(o);
    return true;
  } else if (o.is_numeric()) {
    val = int64_t(o);
    return true;
  }
  return false;
}

bool get_member_uint(const Hjson::Value& doc, const std::string& objName, uint8_t& val) {
  auto o = doc[objName];
  if (!o.defined()) {
    return false;
  }

  if (o.type() == Hjson::Type::String) {
    val = uint8_t(std::atoi(o));
    return true;
  } else if (o.is_numeric()) {
    val = uint8_t(o);
    return true;
  }
  return false;
}

bool get_member_uint(const Hjson::Value& doc, const std::string& objName, uint16_t& val) {
  auto o = doc[objName];
  if (!o.defined()) {
    return false;
  }

  if (o.type() == Hjson::Type::String) {
    val = uint16_t(std::atoi(o));
    return true;
  } else if (o.is_numeric()) {
    val = uint16_t(o);
    return true;
  }
  return false;
}

bool get_member_uint(const Hjson::Value& doc, const std::string& objName, uint32_t& val) {
  auto o = doc[objName];
  if (!o.defined()) {
    return false;
  }

  if (o.type() == Hjson::Type::String) {
    val = uint32_t(std::atoll(o));
    return true;
  } else if (o.is_numeric()) {
    val = uint32_t(o);
    return true;
  }
  return false;
}

bool get_member_uint(const Hjson::Value& doc, const std::string& objName, uint64_t& val) {
  auto o = doc[objName];
  if (!o.defined()) {
    return false;
  }

  if (o.type() == Hjson::Type::String) {
    val = uint64_t(std::atoll(o));
    return true;
  } else if (o.is_numeric()) {
    val = uint64_t(o);
    return true;
  }
  return false;
}

bool get_member_float(const Hjson::Value& doc, const std::string& objName, float& val) {
  auto o = doc[objName];
  if (!o.defined()) {
    return false;
  }

  if (o.type() == Hjson::Type::String) {
    val = float(std::atof(o));
    return true;
  } else if (o.is_numeric()) {
    val = float(o);
    return true;
  }
  return false;
}

bool get_member_double(const Hjson::Value& doc, const std::string& objName, double& val) {
  auto o = doc[objName];
  if (!o.defined()) {
    return false;
  }

  if (o.type() == Hjson::Type::String) {
    val = std::atof(o);
    return true;
  } else if (o.is_numeric()) {
    val = o;
    return true;
  }
  return false;
}

bool get_member_bool(const Hjson::Value& doc, const std::string& objName, bool& val) {
  auto o = doc[objName];
  if (!o.defined()) {
    return false;
  }

  if (o.type() == Hjson::Type::String) {
    val = o == std::string("true");
    return true;
  } else if (o.type() == Hjson::Type::Bool) {
    val = o.operator bool();
    return true;
  }
  return false;
}

bool get_member_bool_relaxed(const Hjson::Value& doc, const std::string& objName, bool& val) {
  auto o = doc[objName];
  if (!o.defined()) {
    return false;
  }

  if (o.type() == Hjson::Type::String) {
    val = o == std::string("true");
    return true;
  } else if (o.type() == Hjson::Type::Bool) {
    val = o.operator bool();
    return true;
  } else if (o.is_numeric()) {
    val = !(std::floor(o.operator double()) == 0);
    return true;
  }
  return false;
}

bool get_member_string(const Hjson::Value& doc, const std::string& objName, std::string& val) {
  auto o = doc[objName];
  if (!o.defined()) {
    return false;
  }

  if (o.type() == Hjson::Type::String) {
    val = o.operator std::string();
    return true;
  }
  return false;
}

bool get_value_int(const Hjson::Value& o, int8_t& val) {
  if (!o.defined()) {
    return false;
  }

  if (o.type() == Hjson::Type::String) {
    val = int8_t(std::atoi(o));
    return true;
  } else if (o.is_numeric()) {
    val = int8_t(o.operator char());
    return true;
  }
  return false;
}

bool get_value_int(const Hjson::Value& o, int16_t& val) {
  if (!o.defined()) {
    return false;
  }

  if (o.type() == Hjson::Type::String) {
    val = int16_t(std::atoi(o));
    return true;
  } else if (o.is_numeric()) {
    val = int16_t(o);
    return true;
  }
  return false;
}

bool get_value_int(const Hjson::Value& o, int32_t& val) {
  if (!o.defined()) {
    return false;
  }

  if (o.type() == Hjson::Type::String) {
    val = std::atoi(o);
    return true;
  } else if (o.is_numeric()) {
    val = int32_t(o);
    return true;
  }
  return false;
}

bool get_value_int(const Hjson::Value& o, int64_t& val) {
  if (!o.defined()) {
    return false;
  }

  if (o.type() == Hjson::Type::String) {
    val = std::atoll(o);
    return true;
  } else if (o.is_numeric()) {
    val = int64_t(o);
    return true;
  }
  return false;
}

bool get_value_uint(const Hjson::Value& o, uint8_t& val) {
  if (!o.defined()) {
    return false;
  }

  if (o.type() == Hjson::Type::String) {
    val = uint8_t(std::atoi(o));
    return true;
  } else if (o.is_numeric()) {
    val = uint8_t(o);
    return true;
  }
  return false;
}

bool get_value_uint(const Hjson::Value& o, uint16_t& val) {
  if (!o.defined()) {
    return false;
  }

  if (o.type() == Hjson::Type::String) {
    val = uint16_t(std::atoi(o));
    return true;
  } else if (o.is_numeric()) {
    val = uint16_t(o);
    return true;
  }
  return false;
}

bool get_value_uint(const Hjson::Value& o, uint32_t& val) {
  if (!o.defined()) {
    return false;
  }

  if (o.type() == Hjson::Type::String) {
    val = uint32_t(std::atoll(o));
    return true;
  } else if (o.is_numeric()) {
    val = uint32_t(o);
    return true;
  }
  return false;
}

bool get_value_uint(const Hjson::Value& o, uint64_t& val) {
  if (!o.defined()) {
    return false;
  }

  if (o.type() == Hjson::Type::String) {
    val = uint64_t(std::atoll(o));
    return true;
  } else if (o.is_numeric()) {
    val = uint64_t(o);
    return true;
  }
  return false;
}

bool get_value_float(const Hjson::Value& o, float& val) {
  if (!o.defined()) {
    return false;
  }

  if (o.type() == Hjson::Type::String) {
    val = float(std::atof(o));
    return true;
  } else if (o.is_numeric()) {
    val = float(o);
    return true;
  }
  return false;
}

bool get_value_double(const Hjson::Value& o, double& val) {
  if (!o.defined()) {
    return false;
  }

  if (o.type() == Hjson::Type::String) {
    val = std::atof(o);
    return true;
  } else if (o.is_numeric()) {
    val = o;
    return true;
  }
  return false;
}

bool get_value_bool(const Hjson::Value& o, bool& val) {
  if (!o.defined()) {
    return false;
  }

  if (o.type() == Hjson::Type::String) {
    val = o == std::string("true");
    return true;
  } else if (o.type() == Hjson::Type::Bool) {
    val = o.operator bool();
    return true;
  }
  return false;
}

bool get_value_string(const Hjson::Value& o, std::string& val) {
  if (!o.defined()) {
    return false;
  }

  if (o.type() == Hjson::Type::String) {
    val = o.operator std::string();
    return true;
  }
  return false;
}

bool get_value_vector(const Hjson::Value& o, std::vector<int8_t>& val) {
  if (!o.defined()) {
    return false;
  }

  if (o.type() == Hjson::Type::Vector) {
    size_t len = o.size();
    val.resize(len);

    bool is_value_extracted = false;
    for (size_t i = 0; i < len; ++i) {
      is_value_extracted = get_value_int(o[int(i)], val[i]);
      if (!is_value_extracted) {
        return false;
      }
    }

    return true;
  }

  return false;
}

bool get_value_vector(const Hjson::Value& o, std::vector<int16_t>& val) {
  if (!o.defined()) {
    return false;
  }

  if (o.type() == Hjson::Type::Vector) {
    size_t len = o.size();
    val.resize(len);

    bool is_value_extracted = false;
    for (size_t i = 0; i < len; ++i) {
      is_value_extracted = get_value_int(o[int(i)], val[i]);
      if (!is_value_extracted) {
        return false;
      }
    }

    return true;
  }

  return false;
}

bool get_value_vector(const Hjson::Value& o, std::vector<int32_t>& val) {
  if (!o.defined()) {
    return false;
  }

  if (o.type() == Hjson::Type::Vector) {
    size_t len = o.size();
    val.resize(len);

    bool is_value_extracted = false;
    for (size_t i = 0; i < len; ++i) {
      is_value_extracted = get_value_int(o[int(i)], val[i]);
      if (!is_value_extracted) {
        return false;
      }
    }

    return true;
  }

  return false;
}

bool get_value_vector(const Hjson::Value& o, std::vector<int64_t>& val) {
  if (!o.defined()) {
    return false;
  }

  if (o.type() == Hjson::Type::Vector) {
    size_t len = o.size();
    val.resize(len);

    bool is_value_extracted = false;
    for (size_t i = 0; i < len; ++i) {
      is_value_extracted = get_value_int(o[int(i)], val[i]);
      if (!is_value_extracted) {
        return false;
      }
    }

    return true;
  }

  return false;
}

bool get_value_vector(const Hjson::Value& o, std::vector<uint8_t>& val) {
  if (!o.defined()) {
    return false;
  }

  if (o.type() == Hjson::Type::Vector) {
    size_t len = o.size();
    val.resize(len);

    bool is_value_extracted = false;
    for (size_t i = 0; i < len; ++i) {
      is_value_extracted = get_value_uint(o[int(i)], val[i]);
      if (!is_value_extracted) {
        return false;
      }
    }

    return true;
  }

  return false;
}

bool get_value_vector(const Hjson::Value& o, std::vector<uint16_t>& val) {
  if (!o.defined()) {
    return false;
  }

  if (o.type() == Hjson::Type::Vector) {
    size_t len = o.size();
    val.resize(len);

    bool is_value_extracted = false;
    for (size_t i = 0; i < len; ++i) {
      is_value_extracted = get_value_uint(o[int(i)], val[i]);
      if (!is_value_extracted) {
        return false;
      }
    }

    return true;
  }

  return false;
}

bool get_value_vector(const Hjson::Value& o, std::vector<uint32_t>& val) {
  if (!o.defined()) {
    return false;
  }

  if (o.type() == Hjson::Type::Vector) {
    size_t len = o.size();
    val.resize(len);

    bool is_value_extracted = false;
    for (size_t i = 0; i < len; ++i) {
      is_value_extracted = get_value_uint(o[int(i)], val[i]);
      if (!is_value_extracted) {
        return false;
      }
    }

    return true;
  }

  return false;
}

bool get_value_vector(const Hjson::Value& o, std::vector<uint64_t>& val) {
  if (!o.defined()) {
    return false;
  }

  if (o.type() == Hjson::Type::Vector) {
    size_t len = o.size();
    val.resize(len);

    bool is_value_extracted = false;
    for (size_t i = 0; i < len; ++i) {
      is_value_extracted = get_value_uint(o[int(i)], val[i]);
      if (!is_value_extracted) {
        return false;
      }
    }

    return true;
  }

  return false;
}

bool get_value_vector(const Hjson::Value& o, std::vector<float>& val) {
  if (!o.defined()) {
    return false;
  }

  if (o.type() == Hjson::Type::Vector) {
    size_t len = o.size();
    val.resize(len);

    bool is_value_extracted = false;
    for (size_t i = 0; i < len; ++i) {
      is_value_extracted = get_value_float(o[int(i)], val[i]);
      if (!is_value_extracted) {
        return false;
      }
    }

    return true;
  }

  return false;
}

bool get_value_vector(const Hjson::Value& o, std::vector<double>& val) {
  if (!o.defined()) {
    return false;
  }

  if (o.type() == Hjson::Type::Vector) {
    size_t len = o.size();
    val.resize(len);

    bool is_value_extracted = false;
    for (size_t i = 0; i < len; ++i) {
      is_value_extracted = get_value_double(o[int(i)], val[i]);
      if (!is_value_extracted) {
        return false;
      }
    }

    return true;
  }

  return false;
}

bool get_value_bool_deque(const Hjson::Value& o, std::deque<bool>& val) {
  if (!o.defined()) {
    return false;
  }

  if (o.type() == Hjson::Type::Vector) {
    size_t len = o.size();
    val.resize(len);

    bool is_value_extracted = false;
    for (size_t i = 0; i < len; ++i) {
      is_value_extracted = get_value_bool(o[int(i)], val[i]);
      if (!is_value_extracted) {
        return false;
      }
    }

    return true;
  }

  return false;
}

bool get_value_vector(const Hjson::Value& o, std::vector<std::string>& val) {
  if (!o.defined()) {
    return false;
  }

  if (o.type() == Hjson::Type::Vector) {
    size_t len = o.size();
    val.resize(len);

    bool is_value_extracted = false;
    for (size_t i = 0; i < len; ++i) {
      is_value_extracted = get_value_string(o[int(i)], val[i]);
      if (!is_value_extracted) {
        return false;
      }
    }

    return true;
  }

  return false;
}

bool get_member_vector(const Hjson::Value& doc, const std::string& objName, std::vector<int8_t>& val) {
  auto o = doc[objName];
  if (!o.defined()) {
    return false;
  }

  bool success = get_value_vector(o, val);
  return success;
}

bool get_member_vector(const Hjson::Value& doc, const std::string& objName, std::vector<int16_t>& val) {
  auto o = doc[objName];
  if (!o.defined()) {
    return false;
  }

  bool success = get_value_vector(o, val);
  return success;
}

bool get_member_vector(const Hjson::Value& doc, const std::string& objName, std::vector<int32_t>& val) {
  auto o = doc[objName];
  if (!o.defined()) {
    return false;
  }

  bool success = get_value_vector(o, val);
  return success;
}

bool get_member_vector(const Hjson::Value& doc, const std::string& objName, std::vector<int64_t>& val) {
  auto o = doc[objName];
  if (!o.defined()) {
    return false;
  }

  bool success = get_value_vector(o, val);
  return success;
}

bool get_member_vector(const Hjson::Value& doc, const std::string& objName, std::vector<uint8_t>& val) {
  auto o = doc[objName];
  if (!o.defined()) {
    return false;
  }

  bool success = get_value_vector(o, val);
  return success;
}

bool get_member_vector(const Hjson::Value& doc, const std::string& objName, std::vector<uint16_t>& val) {
  auto o = doc[objName];
  if (!o.defined()) {
    return false;
  }

  bool success = get_value_vector(o, val);
  return success;
}

bool get_member_vector(const Hjson::Value& doc, const std::string& objName, std::vector<uint32_t>& val) {
  auto o = doc[objName];
  if (!o.defined()) {
    return false;
  }

  bool success = get_value_vector(o, val);
  return success;
}

bool get_member_vector(const Hjson::Value& doc, const std::string& objName, std::vector<uint64_t>& val) {
  auto o = doc[objName];
  if (!o.defined()) {
    return false;
  }

  bool success = get_value_vector(o, val);
  return success;
}

bool get_member_vector(const Hjson::Value& doc, const std::string& objName, std::vector<float>& val) {
  auto o = doc[objName];
  if (!o.defined()) {
    return false;
  }

  bool success = get_value_vector(o, val);
  return success;
}

bool get_member_vector(const Hjson::Value& doc, const std::string& objName, std::vector<double>& val) {
  auto o = doc[objName];
  if (!o.defined()) {
    return false;
  }

  bool success = get_value_vector(o, val);
  return success;
}

bool get_member_bool_deque(const Hjson::Value& doc, const std::string& objName, std::deque<bool>& val) {
  auto o = doc[objName];
  if (!o.defined()) {
    return false;
  }

  bool success = get_value_bool_deque(o, val);
  return success;
}

bool get_member_vector(const Hjson::Value& doc, const std::string& objName, std::vector<std::string>& val) {
  auto o = doc[objName];
  if (!o.defined()) {
    return false;
  }

  bool success = get_value_vector(o, val);
  return success;
}
