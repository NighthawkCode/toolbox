#include <gtest/gtest.h>
#include <toolbox/rate.h>

TEST(TestRateToolbox, RateTest) {
  {
    Rate rate;
    for (int i = 0; i < 10; i++) {
      rate.addPoint(i, i);
    }
    EXPECT_EQ(rate.getRate(), 1.0);
  }

  {
    Rate rate;
    for (int i = 0; i < 10; i++) {
      rate.addPoint(i, i + 100.0);
    }
    EXPECT_EQ(rate.getRate(), 1.0);
  }

  {
    Rate rate;
    for (int i = 0; i < 10; i++) {
      rate.addPoint(i, double(i) / 2.0);
    }
    EXPECT_EQ(rate.getRate(), 2.0);
  }

  {
    Rate rate;
    for (int i = 0; i < 10; i++) {
      rate.addPoint(10, i);
    }
    EXPECT_EQ(rate.getRate(), 0.0);
  }

  {
    Rate rate;
    for (int i = 0; i < 10000; i++) {
      rate.addPoint(i / 100, 100.0 + double(i) / 100.0);
    }
    EXPECT_EQ(rate.getRate(), 1.0);
  }

  {
    Rate rate;
    for (int i = 0; i < 10000; i++) {
      rate.addPoint(i / 100, 100.0 + double(i) / 100.0);
    }
    for (int i = 0; i < 20; i++) {
      rate.addPoint(100, 200.0 + double(i) / 100.0);
    }

    EXPECT_EQ(rate.getRate(), 1.0);
  }

  {
    Rate rate;
    for (int i = 0; i < 3; i++) {
      rate.addPoint(i * 100, i);
    }
    EXPECT_EQ(rate.getRate(), 100.0);
  }

  {
    Rate rate;
    for (int i = 0; i < 3; i++) {
      rate.addPoint(i * 100 + 10, i);
    }
    EXPECT_EQ(rate.getRate(), 100.0);
  }
}

int main(int argc, char** argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
