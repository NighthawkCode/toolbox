#include <gtest/gtest.h>

#include <toolbox/file_utils.h>

TEST(TestFileToolbox, FileAccess) {
  EXPECT_TRUE(dir_exists("/home"));
  EXPECT_FALSE(file_exists("/home"));
  // Fie functions do not handle home folder expansion
  EXPECT_FALSE(dir_exists("~/verdant"));
  EXPECT_FALSE(file_exists("~/.bashrc"));
}

int main(int argc, char** argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
