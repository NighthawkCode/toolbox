#include <gtest/gtest.h>
#include <toolbox/hjson_helper.h>

#include <string>

TEST(TestJsonInclude, BasicInclude) {
  Hjson::Value json;
  std::string spath = TEST_PATH;
  const std::string main_json_file = spath + "/file.json";
  EXPECT_TRUE(load_json_file(json, main_json_file));

  EXPECT_TRUE(json["key1"].to_string() == "val0");
  EXPECT_TRUE(json["nodes"]["nodea"].type() == Hjson::Value::VECTOR);
  EXPECT_TRUE(json["nodes"]["a"].to_double() == 1.0);
  EXPECT_TRUE(json["nodes"]["d"].to_string() == "one");
}

TEST(TestJsonInclude, TestErrors) {
  Hjson::Value json;
  std::string spath = TEST_PATH;
  std::string main_json_file = spath + "/file_badinc.json";
  EXPECT_FALSE(load_json_file(json, main_json_file));

  main_json_file = spath + "/file_circinc1.json";
  EXPECT_FALSE(load_json_file(json, main_json_file));
}

int main(int argc, char** argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
