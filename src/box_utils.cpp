#include "toolbox/box_utils.h"

#include <sstream>
#include <vector>

#include "toolbox/file_utils.h"
#include "vlog.h"
namespace verdant {

static const char* DEFAULT_ECU_IP = "10.10.10.51";

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

std::string GetSyncboxIP(std::string& robot_role) {
  std::string ecu_ip = DEFAULT_ECU_IP;
  std::string ip_prefix = "10.10.10.";

  // Set IP address
  // Generic spraybox: 10.10.10.51
  // SprayBox: ecu_ip_postfix = box_id_number + 52, i.e. spraybox0: 10.10.10.2 -> sb0_ecu_ip: 10.10.10.52
  // NavBox: 10.10.10.51
  if (robot_role.find("spraybox") != std::string::npos) {
    const size_t pos = robot_role.find("spraybox");

    // check if the charachter after "spraybox" is white space
    if (std::isspace(robot_role.substr(pos + 8, 1)[0])) {
      // generic spraybox
      ecu_ip = DEFAULT_ECU_IP;
    } else {
      // spraybox with a box id
      size_t box_id_index = robot_role.find_first_of("0123456789");
      std::string box_id_str = robot_role.substr(box_id_index);
      TrimString(box_id_str);
      const size_t box_id = std::stoi(box_id_str);
      ecu_ip = ip_prefix + std::to_string(box_id + 52);
    }
  } else if (robot_role.find("navbox") != std::string::npos) {
    ecu_ip = DEFAULT_ECU_IP;
  } else {
    ecu_ip = DEFAULT_ECU_IP;
    vlog_warning(VCAT_GENERAL, "Unknown or generic robot role: %s, returning default ECU IP: %s ",
                 robot_role.c_str(), ecu_ip.c_str());
  }

  vlog_fine(VCAT_GENERAL, "Got robot role: %s, corresponding ECU IP: %s", robot_role.c_str(), ecu_ip.c_str());

  return ecu_ip;
}

std::string GetSyncboxIP() {
  std::string robot_role_file = "/etc/robot_role";
  std::string robot_role = "";

  // read robot role
  if (!ReadFileIntoString(robot_role_file, robot_role)) {
    vlog_error(VCAT_GENERAL, "Couldn't read robot role from %s, returning default ip: %s",
               robot_role_file.c_str(), DEFAULT_ECU_IP);
    robot_role = DEFAULT_ECU_IP;
  }

  return GetSyncboxIP(robot_role);
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

std::string ExtractTopicNoBox(const std::string& topic_name) {
  size_t pos_slash = topic_name.find_first_of("/");
  if (pos_slash != std::string::npos) {
    return topic_name.substr(pos_slash + 1);
  } else {
    vlog_warning(VCAT_GENERAL, "[ExtractTopicNoBox] There is no Boxname to be removed from given topic: %s",
                 topic_name.c_str());
    return topic_name;
  }
}
}  // namespace verdant
