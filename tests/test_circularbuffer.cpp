#include <gtest/gtest.h>
#include <toolbox/circularbuffer.h>

template <class T>
void printBuffer(const T& buf) {
  std::string txt;
  for (int i = 0; i < (int)buf.size(); i++) {
    txt += std::to_string(buf[i]) + ",";
  }
  printf("Buffer: %s \n", txt.c_str());
}

TEST(TestCircularBuffer, CircularBuffer) {
  CircularBuffer<int, 10> cb;
  EXPECT_TRUE(cb.size() == 0);
  EXPECT_TRUE(cb.empty());
  EXPECT_FALSE(cb.full());

  cb.push_back(1);
  cb.push_back(2);
  cb.push_back(3);
  cb.push_back(4);
  cb.push_back(5);

  EXPECT_EQ(cb.size(), 5);
  EXPECT_FALSE(cb.empty());
  EXPECT_FALSE(cb.full());

  EXPECT_EQ(cb[0], 5);
  EXPECT_EQ(cb[1], 4);
  EXPECT_EQ(cb[2], 3);
  EXPECT_EQ(cb[3], 2);
  EXPECT_EQ(cb[4], 1);
  EXPECT_EQ(cb.front(), 1);

  // Now fill the buffer
  cb.push_back(6);
  cb.push_back(7);
  cb.push_back(8);
  cb.push_back(9);
  EXPECT_FALSE(cb.full());
  cb.push_back(10);
  EXPECT_TRUE(cb.full());
  EXPECT_EQ(cb[0], 10);

  EXPECT_EQ(cb[0], 10);
  EXPECT_EQ(cb[1], 9);
  EXPECT_EQ(cb[2], 8);
  EXPECT_EQ(cb[3], 7);
  EXPECT_EQ(cb[4], 6);
  EXPECT_EQ(cb[5], 5);
  EXPECT_EQ(cb[6], 4);
  EXPECT_EQ(cb[7], 3);
  EXPECT_EQ(cb[8], 2);
  EXPECT_EQ(cb[9], 1);
  EXPECT_EQ(cb.front(), 1);

  // This push should kick out the oldest element
  cb.push_back(11);
  EXPECT_EQ(cb[0], 11);
  EXPECT_EQ(cb[1], 10);
  EXPECT_EQ(cb[2], 9);
  EXPECT_EQ(cb[3], 8);
  EXPECT_EQ(cb[4], 7);
  EXPECT_EQ(cb[5], 6);
  EXPECT_EQ(cb[6], 5);
  EXPECT_EQ(cb[7], 4);
  EXPECT_EQ(cb[8], 3);
  EXPECT_EQ(cb[9], 2);
  EXPECT_EQ(cb.front(), 2);

  auto& last = cb.emplace_back();
  last = 5;
  EXPECT_EQ(last, cb.back());

  CircularBuffer<int, 5> sp;
  int& a = sp.emplace_back();
  a = 3;
  int& b = sp.emplace_back();
  b = 5;
  int& c = sp.emplace_back();
  c = 7;
  EXPECT_EQ(sp[0], 7);
  EXPECT_EQ(sp[1], 5);
  EXPECT_EQ(sp[2], 3);
  sp.purge(1);
  EXPECT_EQ(sp[0], 7);
  EXPECT_EQ(sp[1], 5);
  EXPECT_EQ(sp.size(), 2);
  int& oth = sp.emplace_back();
  oth = 9;
  EXPECT_EQ(sp[0], 9);
  EXPECT_EQ(sp[1], 7);
  EXPECT_EQ(sp[2], 5);
}

TEST(TestCircularBufferVar, CircularBuffer) {
  CircularBufferVar<int> cb(0);
  EXPECT_EQ(cb.size(), 0);
  EXPECT_EQ(cb.max_size(), 0);
  EXPECT_TRUE(cb.empty());
  EXPECT_TRUE(cb.full());

  cb.resize(10);
  EXPECT_EQ(cb.size(), 0);
  EXPECT_EQ(cb.max_size(), 10);
  EXPECT_TRUE(cb.empty());
  EXPECT_FALSE(cb.full());

  cb.push_back(1);
  cb.push_back(2);
  cb.push_back(3);
  cb.push_back(4);
  cb.push_back(5);

  EXPECT_EQ(cb.size(), 5);
  EXPECT_FALSE(cb.empty());
  EXPECT_FALSE(cb.full());

  EXPECT_EQ(cb[0], 5);
  EXPECT_EQ(cb[1], 4);
  EXPECT_EQ(cb[2], 3);
  EXPECT_EQ(cb[3], 2);
  EXPECT_EQ(cb[4], 1);
  EXPECT_EQ(cb.front(), 1);

  // Now fill the buffer
  cb.push_back(6);
  cb.push_back(7);
  cb.push_back(8);
  cb.push_back(9);
  EXPECT_FALSE(cb.full());
  cb.push_back(10);
  EXPECT_TRUE(cb.full());
  EXPECT_EQ(cb[0], 10);

  EXPECT_EQ(cb[0], 10);
  EXPECT_EQ(cb[1], 9);
  EXPECT_EQ(cb[2], 8);
  EXPECT_EQ(cb[3], 7);
  EXPECT_EQ(cb[4], 6);
  EXPECT_EQ(cb[5], 5);
  EXPECT_EQ(cb[6], 4);
  EXPECT_EQ(cb[7], 3);
  EXPECT_EQ(cb[8], 2);
  EXPECT_EQ(cb[9], 1);
  EXPECT_EQ(cb.front(), 1);

  // This push should kick out the oldest element
  cb.push_back(11);
  EXPECT_EQ(cb[0], 11);
  EXPECT_EQ(cb[1], 10);
  EXPECT_EQ(cb[2], 9);
  EXPECT_EQ(cb[3], 8);
  EXPECT_EQ(cb[4], 7);
  EXPECT_EQ(cb[5], 6);
  EXPECT_EQ(cb[6], 5);
  EXPECT_EQ(cb[7], 4);
  EXPECT_EQ(cb[8], 3);
  EXPECT_EQ(cb[9], 2);
  EXPECT_EQ(cb.front(), 2);

  auto& last = cb.emplace_back();
  last = 5;
  EXPECT_EQ(last, cb.back());
}

int main(int argc, char** argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
