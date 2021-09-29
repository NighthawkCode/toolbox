#pragma once

#include <iomanip>
#include <sstream>
#include <string>
#include <vector>

namespace verdant {

std::string RobotRoleToIp(const std::string& robot_role);

std::string GetPeerList(const std::vector<std::string>& box_names);

std::string ExtractBoxName(const std::string& topic_name);

static inline std::string GetBoxSpecificNodeName(const std::string& box_name, const std::string& node_name) {
  return box_name + "-" + node_name;
}

static inline std::string StripBoxSpecificName(const std::string& node_name) {
  auto pos = node_name.find('-');
  if (pos != std::string::npos) {
    return node_name.substr(pos + 1);
  } else {
    return node_name;
  }
}

static inline std::string GetCurrentTimeString() {
  auto t = std::time(nullptr);
  auto tm = *std::localtime(&t);
  std::stringstream ts;
  ts << std::put_time(&tm, "%Y-%m-%dT%H:%M:%S.%z%Z");
  return ts.str();
}

}  // namespace verdant
