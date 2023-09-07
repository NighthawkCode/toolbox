#include <gtest/gtest.h>
#include <toolbox/file_utils.h>

TEST(TestFileToolbox, FileAccess) {
  EXPECT_TRUE(dir_exists("/home"));
  EXPECT_FALSE(file_exists("/home"));
  // Fie functions do not handle home folder expansion
  EXPECT_FALSE(dir_exists("~/verdant"));
  EXPECT_FALSE(file_exists("~/.bashrc"));
}

TEST(TestFileToolbox, ReadFile) {
  std::string spath = std::string(TEST_PATH) + "/one_word.txt";
  std::string contents = ReadFileIntoString(spath);
  printf("The contents are %s", contents.c_str());
  ASSERT_TRUE(contents == "test_word\n");
}

TEST(TestFileToolbox, StringTrim) {
  std::string str_with_space = " test ";
  TrimString(str_with_space);
  ASSERT_TRUE(str_with_space == "test");
}

int main(int argc, char** argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
