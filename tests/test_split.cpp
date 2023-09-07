#include <gtest/gtest.h>

#include "toolbox/split.h"

using namespace toolbox;

TEST(TestSplit, empty) {
  const auto result_vec = split("", ',');
  EXPECT_EQ(result_vec.size(), 1);
  EXPECT_EQ(result_vec.at(0), "");
}

TEST(TestSplit, onesep) {
  const auto result_vec = split(",", ',');
  ASSERT_EQ(result_vec.size(), 2);
  EXPECT_EQ(result_vec.at(0), "");
  EXPECT_EQ(result_vec.at(1), "");
}

TEST(TestSplit, leading_sep) {
  const auto result_vec = split(",foo", ',');
  ASSERT_EQ(result_vec.size(), 2);
  EXPECT_EQ(result_vec.at(0), "");
  EXPECT_EQ(result_vec.at(1), "foo");
}

TEST(TestSplit, trailing_sep) {
  const auto result_vec = split("foo,", ',');
  ASSERT_EQ(result_vec.size(), 2);
  EXPECT_EQ(result_vec.at(0), "foo");
  EXPECT_EQ(result_vec.at(1), "");
}

TEST(TestSplit, trailing_sep2) {
  const auto result_vec = split("foo,,", ',');
  ASSERT_EQ(result_vec.size(), 3);
  EXPECT_EQ(result_vec.at(0), "foo");
  EXPECT_EQ(result_vec.at(1), "");
  EXPECT_EQ(result_vec.at(2), "");
}

TEST(TestSplit, normal) {
  const auto result_vec = split("foo/bar/baz/quux", '/');
  ASSERT_EQ(result_vec.size(), 4);
  EXPECT_EQ(result_vec.at(0), "foo");
  EXPECT_EQ(result_vec.at(1), "bar");
  EXPECT_EQ(result_vec.at(2), "baz");
  EXPECT_EQ(result_vec.at(3), "quux");
}

TEST(TestIntRange, empty) {
  std::vector<int> output_val;
  const bool success = split_int_range("", output_val);
  EXPECT_FALSE(success);
  EXPECT_EQ(output_val.size(), 0);
}

TEST(TestIntRange, bad_input) {
  std::vector<int> output_val;
  const bool success = split_int_range("1,2,3,bad", output_val);
  EXPECT_FALSE(success);
}

TEST(TestIntRange, one_number) {
  std::vector<int> output_val;
  const bool success = split_int_range("10", output_val);
  EXPECT_TRUE(success);
  EXPECT_EQ(output_val.size(), 1);
  EXPECT_EQ(output_val.at(0), 10);
}

TEST(TestIntRange, comma_sep) {
  std::vector<int> output_val;
  const bool success = split_int_range("1,2,3", output_val);
  EXPECT_TRUE(success);
  EXPECT_EQ(output_val.size(), 3);
  EXPECT_EQ(output_val.at(0), 1);
  EXPECT_EQ(output_val.at(1), 2);
  EXPECT_EQ(output_val.at(2), 3);
}

TEST(TestIntRange, comma_sep_negative) {
  std::vector<int> output_val;
  const bool success = split_int_range("-1,-2,-3", output_val);
  EXPECT_TRUE(success);
  EXPECT_EQ(output_val.size(), 3);
  EXPECT_EQ(output_val.at(0), -1);
  EXPECT_EQ(output_val.at(1), -2);
  EXPECT_EQ(output_val.at(2), -3);
}

TEST(TestIntRange, ranges) {
  std::vector<int> output_val;
  const bool success = split_int_range("-5:-2,100:102", output_val);
  EXPECT_TRUE(success);
  EXPECT_EQ(output_val.size(), 7);
  EXPECT_EQ(output_val.at(0), -5);
  EXPECT_EQ(output_val.at(1), -4);
  EXPECT_EQ(output_val.at(2), -3);
  EXPECT_EQ(output_val.at(3), -2);
  EXPECT_EQ(output_val.at(4), 100);
  EXPECT_EQ(output_val.at(5), 101);
  EXPECT_EQ(output_val.at(6), 102);
}

TEST(TestIntRange, bad_values) {
  std::vector<int> output;
  EXPECT_EQ(split_int_range("", output), false);
  EXPECT_EQ(split_int_range("00", output), false);
  EXPECT_EQ(split_int_range("0-", output), false);
  EXPECT_EQ(split_int_range("0+", output), false);
  EXPECT_EQ(split_int_range("0.0", output), false);
  EXPECT_EQ(split_int_range("bad", output), false);
}

TEST(TestIntRange, good_zeros) {
  {
    std::vector<int> output;
    EXPECT_EQ(split_int_range("+0", output), true);
    ASSERT_EQ(output.size(), 1);
    EXPECT_EQ(output.at(0), 0);
  }
  {
    std::vector<int> output;
    EXPECT_EQ(split_int_range("-0", output), true);
    ASSERT_EQ(output.size(), 1);
    EXPECT_EQ(output.at(0), 0);
  }
  {
    std::vector<int> output;
    EXPECT_EQ(split_int_range("0", output), true);
    ASSERT_EQ(output.size(), 1);
    EXPECT_EQ(output.at(0), 0);
  }
  {
    std::vector<int> output;
    EXPECT_EQ(split_int_range(" +0 ", output), true);
    ASSERT_EQ(output.size(), 1);
    EXPECT_EQ(output.at(0), 0);
  }
  {
    std::vector<int> output;
    EXPECT_EQ(split_int_range(" -0 ", output), true);
    ASSERT_EQ(output.size(), 1);
    EXPECT_EQ(output.at(0), 0);
  }
  {
    std::vector<int> output;
    EXPECT_EQ(split_int_range(" 0 ", output), true);
    ASSERT_EQ(output.size(), 1);
    EXPECT_EQ(output.at(0), 0);
  }
}

int main(int argc, char** argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
