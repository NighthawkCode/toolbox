#include <gtest/gtest.h>
#include <toolbox/box_utils.h>

TEST(TestBoxToolbox, SyncBoxIP) {
  std::string robot_role = "spraybox10 ";
  std::string ecu_ip = verdant::GetSyncboxIP(robot_role);
  ASSERT_TRUE(ecu_ip == "10.10.10.62");
}

int main(int argc, char** argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
