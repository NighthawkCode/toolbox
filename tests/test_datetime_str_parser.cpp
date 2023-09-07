#include <gtest/gtest.h>
#include <toolbox/datetime_str_parser.h>

TEST(TestDatetimeStrParser, TestFullDatetimeStr) {
  const char datetime_str[] = "2021-06-01T16:05:07.000001";
  std::optional<double> timestamp = parse_datetime_str_utc(datetime_str);
  EXPECT_EQ(timestamp, 1622563507.000001);
}

TEST(TestDatetimeStrParser, TestDatetimeStrOmitFraction) {
  const char datetime_str[] = "2021-06-01T16:05:07";
  std::optional<double> timestamp = parse_datetime_str_utc(datetime_str);
  EXPECT_EQ(timestamp, 1622563507);
}

TEST(TestDatetimeStrParser, TestDatetimeStrOmitMinSec) {
  const char datetime_str[] = "2021-06-01T16";
  std::optional<double> timestamp = parse_datetime_str_utc(datetime_str);
  EXPECT_EQ(timestamp, 1622563200);
}

TEST(TestDatetimeStrParser, TestDatetimeStrOmitHourMinSec) {
  const char datetime_str[] = "2021-06-01";
  std::optional<double> timestamp = parse_datetime_str_utc(datetime_str);
  EXPECT_EQ(timestamp, 1622505600);
}

TEST(TestDatetimeStrParser, TestInvalidDatetimeStr) {
  const char datetime_str[] = "2021-06-01X";
  std::optional<double> timestamp = parse_datetime_str_utc(datetime_str);
  EXPECT_FALSE(timestamp.has_value());
}

TEST(TestDatetimeStrParser, TestNumberStr) {
  const char datetime_str[] = "1622563507.000001";
  std::optional<double> timestamp = parse_datetime_str_utc(datetime_str);
  EXPECT_EQ(timestamp, 1622563507.000001);
}

TEST(TestDatetimeStrParser, TestNumberStr2) {
  const char datetime_str[] = "1.1e9";
  std::optional<double> timestamp = parse_datetime_str_utc(datetime_str);
  EXPECT_EQ(timestamp, 1100000000.);
}

TEST(TestDatetimeStrParser, TestInvalidNumberStr) {
  const char datetime_str[] = "1622563507X";
  std::optional<double> timestamp = parse_datetime_str_utc(datetime_str);
  EXPECT_FALSE(timestamp.has_value());
}

int main(int argc, char** argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
