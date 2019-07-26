#pragma once
#include <hjson.h>

bool check_property_string( const char *parser, const Hjson::Value &o, const char *prop );
bool check_property_obj( const char *parser, const Hjson::Value &o, const char *prop );
bool check_property_bool( const char *parser, const Hjson::Value &o, const char *prop );
void get_property_int(const Hjson::Value& o, int &val);
void get_property_float(const Hjson::Value& o, float &val);
void get_property_bool(const Hjson::Value& o, bool &val);
bool get_member_int(const Hjson::Value& doc, const std::string& objName, int &val);
bool get_member_float(const Hjson::Value& doc, const std::string& objName, float &val);
bool get_member_bool(const Hjson::Value& doc, const std::string& objName, bool &val);
bool get_member_string(const Hjson::Value& doc, const std::string& objName, std::string& val);
bool has_member(const Hjson::Value& doc, const std::string& objName);

namespace verdant
{
  /// Get a json device value based on the sensor name. 
  bool has_device(
      const std::string& sensor_name,   //< Input: 
      const std::string& type,          //< Input: lidar, camera, etc
      const Hjson::Value& json  //< Input:
      );

  // it is assumed you've called the above "has" function first.  sigh.
  Hjson::Value get_device(
      const std::string& sensor_name,   //< Input: 
      const std::string& type,          //< Input: lidar, camera, etc
      const Hjson::Value& json  //< Input:
      );
}

