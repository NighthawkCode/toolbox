#pragma once
#include <hjson.h>

bool load_json(Hjson::Value &json, const std::string& text);
bool check_property_string( const char *parser, const Hjson::Value &o, const char *prop );
bool check_property_obj( const char *parser, const Hjson::Value &o, const char *prop );
bool check_property_bool( const char *parser, const Hjson::Value &o, const char *prop );

void get_property_int(const Hjson::Value& o, int &val);
void get_property_float(const Hjson::Value& o, float &val);
void get_property_bool( const Hjson::Value& o, bool &val, bool default_value = false );
bool get_value_int(const Hjson::Value& doc, int8_t &val);
bool get_value_int(const Hjson::Value& doc, int16_t &val);
bool get_value_int(const Hjson::Value& doc, int32_t &val);
bool get_value_int(const Hjson::Value& doc, int64_t &val);
bool get_value_uint(const Hjson::Value& doc, uint8_t &val);
bool get_value_uint(const Hjson::Value& doc, uint16_t &val);
bool get_value_uint(const Hjson::Value& doc, uint32_t &val);
bool get_value_uint(const Hjson::Value& doc, uint64_t &val);
bool get_value_float(const Hjson::Value& doc, float &val);
bool get_value_double(const Hjson::Value& doc, double &val);
bool get_value_bool(const Hjson::Value& doc, bool &val);
bool get_value_string(const Hjson::Value& doc, std::string& val);
bool get_member_int(const Hjson::Value& doc, const std::string& objName, int8_t &val);
bool get_member_int(const Hjson::Value& doc, const std::string& objName, int16_t &val);
bool get_member_int(const Hjson::Value& doc, const std::string& objName, int32_t &val);
bool get_member_int(const Hjson::Value& doc, const std::string& objName, int64_t &val);
bool get_member_uint(const Hjson::Value& doc, const std::string& objName, uint8_t &val);
bool get_member_uint(const Hjson::Value& doc, const std::string& objName, uint16_t &val);
bool get_member_uint(const Hjson::Value& doc, const std::string& objName, uint32_t &val);
bool get_member_uint(const Hjson::Value& doc, const std::string& objName, uint64_t &val);
bool get_member_float(const Hjson::Value& doc, const std::string& objName, float &val);
bool get_member_double(const Hjson::Value& doc, const std::string& objName, double &val);
bool get_member_bool(const Hjson::Value& doc, const std::string& objName, bool &val);
bool get_member_string(const Hjson::Value& doc, const std::string& objName, std::string& val);
bool has_member(const Hjson::Value& doc, const std::string& objName);
