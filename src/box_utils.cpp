#include "toolbox/box_utils.h"

#include <sstream>
#include <vector>

#include "vlog.h"
namespace verdant {

std::string RobotRoleToIp(const std::string& robot_role) {
  const std::string kSubnet = "10.10.10.";
  const std::string kUnknownIp = "UNKNOWN_ROLE_" + robot_role;

  if (robot_role.compare("navbox") == 0) {
    return kSubnet + "1";
  } else if (robot_role.find("spraybox") != std::string::npos) {
    const size_t pos = robot_role.find("spraybox");
    const size_t box_id = std::stoi(robot_role.substr(pos + 8, 1));
    return kSubnet + std::to_string(box_id + 2);
  } else if (robot_role.compare("tablet") == 0) {
    return kSubnet + "20";
  }

  return kUnknownIp;
}

std::string GetPeerList(const std::vector<std::string>& box_names) {
  std::stringstream ss;
  for (size_t i = 0; i < box_names.size(); i++) {
    const auto& box_name = box_names[i];
    vlog_debug(VCAT_GENERAL, "%s", verdant::RobotRoleToIp(box_name).c_str());
    ss << verdant::RobotRoleToIp(box_name);
    if (i != box_names.size() - 1) {
      ss << ";";
    }
  }
  return ss.str();
}

std::string ExtractBoxName(const std::string& topic_name) {
  std::string boxname = "";
  size_t pos_slash = topic_name.find_first_of("/");
  if (pos_slash != std::string::npos) {
    boxname = topic_name.substr(0, pos_slash);
  } else {
    vlog_error(VCAT_GENERAL, "[ExtractBoxName] Boxname cannot be inferred from given topic: %s",
               topic_name.c_str());
    return "";
  }

  return boxname;
}
}  // namespace verdant
