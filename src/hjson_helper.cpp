#include "types.h"
#include "hjson_helper.h"
#include "vlog.h"
#include <assert.h>

bool load_json(Hjson::Value &json, const std::string& text)
{
  try {
    json = Hjson::Unmarshal( text.c_str() );
    return true;
  } catch (...) {
    vlog_error(VCAT_GENERAL, "Could not parse json configuration file %s", text.c_str() );
    return false;
  }
}

bool has_member(const Hjson::Value& doc, const std::string& objName)
{
  auto v = doc[objName];
  if (!v.defined()) {
    return false;
  }
  return true;
}


bool check_property_string(const char *parser, const Hjson::Value &o, const char *prop)
{
  if (!has_member(o, prop)) {
    vlog_error(VCAT_GENERAL, "%s configuration needs to have a '%s' property\n", parser, prop);
    return false;
  }
  if (o[prop].type() != Hjson::Value::STRING) {
    vlog_error(VCAT_GENERAL, "%s configuration, '%s' needs to be a string\n", parser, prop);
    return false;
  }
  return true;
}

bool check_property_bool(const char *parser, const Hjson::Value &o, const char *prop)
{
  if (!has_member(o, prop)) {
    vlog_error(VCAT_GENERAL, "%s configuration file needs to have a '%s' property\n", parser, prop);
    return false;
  }
  if (o[prop].type() != Hjson::Value::BOOL) {
    vlog_error(VCAT_GENERAL, "%s configuration file, '%s' needs to be a Bool\n", parser, prop);
    return false;
  }
  return true;
}

bool check_property_obj(const char *parser, const Hjson::Value &o, const char *prop)
{
  if (!has_member(o, prop)) {
    vlog_error(VCAT_GENERAL, "Logger configuration file needs to have a '%s' property\n", prop);
    return false;
  }
  if (o[prop].type() != Hjson::Value::MAP) {
    vlog_error(VCAT_GENERAL, "Logger configuration file, '%s' needs to be an object\n", prop);
    return false;
  }
  return true;
}

void get_property_int(const Hjson::Value& o, int &val)
{
  if (o.type() == Hjson::Value::STRING) {
    std::string v = o;
    val = std::atoi(o);
  }
  else if (o.type() == Hjson::Value::DOUBLE) {
    val = int(o);
  }
}

void get_property_float(const Hjson::Value& o, float &val)
{
  if (o.type() == Hjson::Value::STRING) {
    std::string v = o;
    val = std::atof(o);
  } else if (o.type() == Hjson::Value::DOUBLE) {
    val = o;
  }
}

void get_property_bool(const Hjson::Value& o, bool &val, bool default_value )
{
  if (!o.defined()) {
    val = default_value;
    return;
  }
  if (o.type() == Hjson::Value::STRING) {
    val = (o == std::string("true") );
  } else if (o.type() == Hjson::Value::BOOL ){
    val = o.operator bool();
  } else if (o.type() == Hjson::Value::DOUBLE) {
    int v = int(o);
    val = bool(v);
  }
}

bool get_member_int(const Hjson::Value& doc, const std::string& objName, int8_t &val)
{
  auto o = doc[objName];
  if (!o.defined()) {
    return false;
  }

  if (o.type() == Hjson::Value::STRING) {
    val = std::atoi(o);
    return true;
  } else if (o.type() == Hjson::Value::DOUBLE) {
    val = int8_t(o);
    return true;
  }
  return false;
}

bool get_member_int(const Hjson::Value& doc, const std::string& objName, int16_t &val)
{
  auto o = doc[objName];
  if (!o.defined()) {
    return false;
  }

  if (o.type() == Hjson::Value::STRING) {
    val = std::atoi(o);
    return true;
  } else if (o.type() == Hjson::Value::DOUBLE) {
    val = int16_t(o);
    return true;
  }
  return false;
}

bool get_member_int(const Hjson::Value& doc, const std::string& objName, int32_t &val)
{
  auto o = doc[objName];
  if (!o.defined()) {
    return false;
  }

  if (o.type() == Hjson::Value::STRING) {
    val = std::atoi(o);
    return true;
  } else if (o.type() == Hjson::Value::DOUBLE) {
    val = int32_t(o);
    return true;
  }
  return false;
}

bool get_member_int(const Hjson::Value& doc, const std::string& objName, int64_t &val)
{
  auto o = doc[objName];
  if (!o.defined()) {
    return false;
  }

  if (o.type() == Hjson::Value::STRING) {
    val = std::atoi(o);
    return true;
  } else if (o.type() == Hjson::Value::DOUBLE) {
    val = int64_t(o);
    return true;
  }
  return false;
}

bool get_member_uint(const Hjson::Value& doc, const std::string& objName, uint8_t &val)
{
  auto o = doc[objName];
  if (!o.defined()) {
    return false;
  }

  if (o.type() == Hjson::Value::STRING) {
    val = std::atoi(o);
    return true;
  } else if (o.type() == Hjson::Value::DOUBLE) {
    val = uint8_t(o);
    return true;
  }
  return false;
}

bool get_member_uint(const Hjson::Value& doc, const std::string& objName, uint16_t &val)
{
  auto o = doc[objName];
  if (!o.defined()) {
    return false;
  }

  if (o.type() == Hjson::Value::STRING) {
    val = std::atoi(o);
    return true;
  } else if (o.type() == Hjson::Value::DOUBLE) {
    val = uint16_t(o);
    return true;
  }
  return false;
}

bool get_member_uint(const Hjson::Value& doc, const std::string& objName, uint32_t &val)
{
  auto o = doc[objName];
  if (!o.defined()) {
    return false;
  }

  if (o.type() == Hjson::Value::STRING) {
    val = std::atoi(o);
    return true;
  } else if (o.type() == Hjson::Value::DOUBLE) {
    val = uint32_t(o);
    return true;
  }
  return false;
}

bool get_member_uint(const Hjson::Value& doc, const std::string& objName, uint64_t &val)
{
  auto o = doc[objName];
  if (!o.defined()) {
    return false;
  }

  if (o.type() == Hjson::Value::STRING) {
    val = std::atoi(o);
    return true;
  } else if (o.type() == Hjson::Value::DOUBLE) {
    val = uint64_t(o);
    return true;
  }
  return false;
}

bool get_member_float(const Hjson::Value& doc, const std::string& objName, float &val)
{
  auto o = doc[objName];
  if (!o.defined()) {
    return false;
  }

  if (o.type() == Hjson::Value::STRING) {
    val = std::atof(o);
    return true;
  } else if (o.type() == Hjson::Value::DOUBLE) {
    val = o;
    return true;
  }
  return false;
}

bool get_member_double(const Hjson::Value& doc, const std::string& objName, double &val)
{
  auto o = doc[objName];
  if (!o.defined()) {
    return false;
  }

  if (o.type() == Hjson::Value::STRING) {
    val = std::atof(o);
    return true;
  } else if (o.type() == Hjson::Value::DOUBLE) {
    val = o;
    return true;
  }
  return false;
}

bool get_member_bool(const Hjson::Value& doc, const std::string& objName, bool &val)
{
  auto o = doc[objName];
  if (!o.defined()) {
    return false;
  }

  if (o.type() == Hjson::Value::STRING) {
    val = o == std::string("true");
    return true;
  } else if (o.type() == Hjson::Value::BOOL) {
    val = o.operator bool();
    return true;
  }
  return false;
}

bool get_member_string(const Hjson::Value& doc, const std::string& objName, std::string& val)
{
  auto o = doc[objName];
  if (!o.defined()) {
    return false;
  }

  if (o.type() == Hjson::Value::STRING) {
    val = o.operator const std::string();
    return true;
  }
  return false;
}

bool get_value_int(const Hjson::Value& o, int8_t &val)
{
  if (!o.defined()) {
    return false;
  }

  if (o.type() == Hjson::Value::STRING) {
    val = std::atoi(o);
    return true;
  } else if (o.type() == Hjson::Value::DOUBLE) {
    val = int8_t(o);
    return true;
  }
  return false;
}

bool get_value_int(const Hjson::Value& o, int16_t &val)
{
  if (!o.defined()) {
    return false;
  }

  if (o.type() == Hjson::Value::STRING) {
    val = std::atoi(o);
    return true;
  } else if (o.type() == Hjson::Value::DOUBLE) {
    val = int16_t(o);
    return true;
  }
  return false;
}

bool get_value_int(const Hjson::Value& o, int32_t &val)
{
  if (!o.defined()) {
    return false;
  }

  if (o.type() == Hjson::Value::STRING) {
    val = std::atoi(o);
    return true;
  } else if (o.type() == Hjson::Value::DOUBLE) {
    val = int32_t(o);
    return true;
  }
  return false;
}

bool get_value_int(const Hjson::Value& o, int64_t &val)
{
  if (!o.defined()) {
    return false;
  }

  if (o.type() == Hjson::Value::STRING) {
    val = std::atoi(o);
    return true;
  } else if (o.type() == Hjson::Value::DOUBLE) {
    val = int64_t(o);
    return true;
  }
  return false;
}

bool get_value_int(const Hjson::Value& o, uint8_t &val)
{
  if (!o.defined()) {
    return false;
  }

  if (o.type() == Hjson::Value::STRING) {
    val = std::atoi(o);
    return true;
  } else if (o.type() == Hjson::Value::DOUBLE) {
    val = uint8_t(o);
    return true;
  }
  return false;
}

bool get_value_int(const Hjson::Value& o, uint16_t &val)
{
  if (!o.defined()) {
    return false;
  }

  if (o.type() == Hjson::Value::STRING) {
    val = std::atoi(o);
    return true;
  } else if (o.type() == Hjson::Value::DOUBLE) {
    val = uint16_t(o);
    return true;
  }
  return false;
}

bool get_value_int(const Hjson::Value& o, uint32_t &val)
{
  if (!o.defined()) {
    return false;
  }

  if (o.type() == Hjson::Value::STRING) {
    val = std::atoi(o);
    return true;
  } else if (o.type() == Hjson::Value::DOUBLE) {
    val = uint32_t(o);
    return true;
  }
  return false;
}

bool get_value_int(const Hjson::Value& o, uint64_t &val)
{
  if (!o.defined()) {
    return false;
  }

  if (o.type() == Hjson::Value::STRING) {
    val = std::atoi(o);
    return true;
  } else if (o.type() == Hjson::Value::DOUBLE) {
    val = uint64_t(o);
    return true;
  }
  return false;
}

bool get_value_float(const Hjson::Value& o, float &val)
{
  if (!o.defined()) {
    return false;
  }

  if (o.type() == Hjson::Value::STRING) {
    val = std::atof(o);
    return true;
  } else if (o.type() == Hjson::Value::DOUBLE) {
    val = o;
    return true;
  }
  return false;
}

bool get_value_double(const Hjson::Value& o, double &val)
{
  if (!o.defined()) {
    return false;
  }

  if (o.type() == Hjson::Value::STRING) {
    val = std::atof(o);
    return true;
  } else if (o.type() == Hjson::Value::DOUBLE) {
    val = o;
    return true;
  }
  return false;
}

bool get_value_bool(const Hjson::Value& o, bool &val)
{
  if (!o.defined()) {
    return false;
  }

  if (o.type() == Hjson::Value::STRING) {
    val = o == std::string("true");
    return true;
  } else if (o.type() == Hjson::Value::BOOL) {
    val = o.operator bool();
    return true;
  }
  return false;
}

bool get_value_string(const Hjson::Value& o, std::string& val)
{
  if (!o.defined()) {
    return false;
  }

  if (o.type() == Hjson::Value::STRING) {
    val = o.operator const std::string();
    return true;
  }
  return false;
}
