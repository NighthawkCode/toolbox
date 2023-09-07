#include <gtest/gtest.h>
#include <toolbox/string_utils.h>

TEST(TestStringsToolbox, fmt) {
  const auto str = toolbox::fmt("Hello, %s! %f.", "World", 3.14);
  EXPECT_EQ(str, "Hello, World! 3.140000.");
}

TEST(TestStringsToolbox, BufferToString) {
  auto str = toolbox::BufferToString(" test!", 5);
  EXPECT_EQ(str, " test");
  str = toolbox::BufferToString(" te\0t!", 5);
  EXPECT_EQ(str, " te");
}

TEST(TestStringsToolbox, StringToBuffer) {
  char buffer[10];
  toolbox::StringToBuffer("test", buffer, 10);
  EXPECT_EQ(std::string(buffer), "test");
  toolbox::StringToBuffer("test", buffer, 4);
  EXPECT_EQ(std::string(buffer), "tes");
}

TEST(TestStringsToolbox, removeTrailingNumbers) {
  std::string sb0 = "spraybox0";
  auto res = toolbox::removeTrailingNumbers(sb0);
  EXPECT_EQ(res, "spraybox");
  std::string nb = "navbox";
  res = toolbox::removeTrailingNumbers(nb);
  EXPECT_EQ(res, "navbox");
}

TEST(TestStringsToolbox, vectorJoin) {
  std::vector<std::string> v = {"a", "b", "c"};
  auto res = toolbox::join(v, ",");
  EXPECT_EQ(res, "a,b,c");
}

int main(int argc, char** argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
