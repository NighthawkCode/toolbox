#include <gtest/gtest.h>

#include "toolbox/percentile_buffer.h"

template <class T>
void print_buffer(const PercentileBuffer<T>& buffer) {
  // print Buffer elements using advance iterator
  std::cout << "Buffer elements (size: " << buffer.size() << " max_size: " << buffer.maxSize() << "): ";
  for (auto it = buffer.begin(); it != buffer.end(); buffer.next(it)) {
    std::cout << it->first << " ";
  }
  std::cout << std::endl;
}

TEST(PercentileBufferTest, AddAndSizeTest) {
  PercentileBuffer<double> buffer(2);  // Set max size to 3 for simplicity
  ASSERT_EQ(buffer.size(), 0);

  // Add elements in descending order
  buffer.add(3.0);
  ASSERT_EQ(buffer.size(), 1);

  buffer.add(2.0);
  ASSERT_EQ(buffer.size(), 2);

  // At this point, the 'left' multimap contains 3.0 and 2.0, and the 'right' multimap is empty.

  // Add 1.0 to the buffer. This will trigger the removal of the oldest element, which is 3.0 in the 'left'
  // multimap. This covers the branch where 'left' is not empty and 'right' is empty.
  buffer.add(1.0);
  ASSERT_EQ(buffer.size(), 2);
}

TEST(PercentileBufferTest, getPercentileTest) {
  PercentileBuffer<double> buffer(
      100);  // max size = 100, 95th percentile element is 95th element when buffer is full
  for (int i = 1; i <= 100; ++i) {
    buffer.add((double)i);
  }
  ASSERT_EQ(buffer.size(), 100);
  ASSERT_EQ(buffer.getPercentileParameter(), 0.95);
  ASSERT_DOUBLE_EQ(buffer.getPercentile(), 95.0);
}

TEST(PercentileBufferTest, getPercentileTest2) {
  PercentileBuffer<double> buffer(
      10);  // max size = 10, 95th percentile element is 10th element when buffer is full

  buffer.add(1.0);
  buffer.add(10.0);
  buffer.add(5.0);
  buffer.add(7.0);
  buffer.add(3.0);
  buffer.add(8.0);
  buffer.add(2.0);
  buffer.add(6.0);
  buffer.add(4.0);
  buffer.add(9.0);

  ASSERT_EQ(buffer.getPercentileParameter(), 0.95);
  ASSERT_DOUBLE_EQ(buffer.getPercentile(), 10.0);

  // add one more element
  buffer.add(0.5);
  ASSERT_EQ(buffer.size(), 10);
  ASSERT_DOUBLE_EQ(buffer.getPercentile(), 10.0);
}

TEST(PercentileBufferTest, getPercentileTest3) {
  // 95th percentile when the buffer is not full
  PercentileBuffer<double> buffer(100);
  for (int i = 1; i <= 50; ++i) {
    buffer.add((double)i);
  }
  ASSERT_EQ(buffer.getPercentileParameter(), 0.95);
  ASSERT_DOUBLE_EQ(buffer.getPercentile(), 48.0);
}

TEST(PercentileBufferTest, ResetTest) {
  PercentileBuffer<double> buffer(10);
  for (int i = 1; i <= 10; ++i) {
    buffer.add(i);
  }
  buffer.reset();
  ASSERT_EQ(buffer.size(), 0);
}

TEST(PercentileBufferTest, FindTest) {
  PercentileBuffer<double> buffer(10);
  for (int i = 1; i <= 10; ++i) {
    buffer.add(i);
  }
  ASSERT_EQ(buffer.find(1.0), 0);
  ASSERT_EQ(buffer.find(10.0), 9);
  ASSERT_EQ(buffer.find(11.0), -1);
}

TEST(PercentileBufferTest, OverflowTest) {
  PercentileBuffer<double> buffer(
      10);  // max size = 10, 95th percentile element is 10th element when buffer is full
  for (int i = 1; i <= 10; ++i) {
    buffer.add(i);
  }
  ASSERT_EQ(buffer.getPercentileParameter(), 0.95);
  ASSERT_DOUBLE_EQ(buffer.getPercentile(), 10);
  buffer.add(11.0);
  ASSERT_DOUBLE_EQ(buffer.getPercentile(), 11);
}

TEST(PercentileBufferTest, RepeatedValuesTest) {
  PercentileBuffer<double> buffer(
      10);  // max size = 10, 95th percentile element is 10th element when buffer is full
  for (int i = 1; i <= 10; ++i) {
    buffer.add(1.0);
  }
  ASSERT_EQ(buffer.getPercentileParameter(), 0.95);
  ASSERT_DOUBLE_EQ(buffer.getPercentile(), 1.0);
  buffer.add(2.0);
  ASSERT_DOUBLE_EQ(buffer.getPercentile(), 2.0);
}

TEST(PercentileBufferTest, SetPercentileParameterTest) {
  PercentileBuffer<double> buffer(100);
  for (int i = 1; i <= 100; ++i) {
    buffer.add(i);
  }
  ASSERT_EQ(buffer.getPercentileParameter(), 0.95);
  ASSERT_DOUBLE_EQ(buffer.getPercentile(), 95.0);

  // check with same percentile parameter
  EXPECT_NO_THROW(buffer.setPercentileParameter(0.95));
  ASSERT_EQ(buffer.getPercentileParameter(), 0.95);

  // change percentile parameter
  buffer.setPercentileParameter(0.5);
  ASSERT_EQ(buffer.getPercentileParameter(), 0.5);
  ASSERT_DOUBLE_EQ(buffer.getPercentile(), 50.0);

  // reset with same percentile parameter
  EXPECT_NO_THROW(buffer.setPercentileParameter(1.0));
  ASSERT_EQ(buffer.getPercentileParameter(), 1.0);

  // set percentile parameter on empty buffer
  buffer.reset();
  EXPECT_NO_THROW(buffer.setPercentileParameter(0.5));
}

TEST(PercentileBufferTest, BeginEndTest) {
  PercentileBuffer<double> buffer(10);
  buffer.add(1.0);
  buffer.add(2.0);
  ASSERT_DOUBLE_EQ(buffer.begin()->first, 1.0);
  auto it = buffer.end();
  buffer.previous(it);
  ASSERT_DOUBLE_EQ(it->first, 2.0);

  // create a const buffer and check begin and end
  const PercentileBuffer<double>& const_buffer = buffer;
  ASSERT_DOUBLE_EQ(const_buffer.begin()->first, 1.0);
  auto const_it = const_buffer.end();
  const_buffer.previous(const_it);
  ASSERT_DOUBLE_EQ(const_it->first, 2.0);
}

TEST(PercentileBufferTest, AdvanceTest) {
  PercentileBuffer<double> buffer(100);
  for (int i = 0; i < 100; ++i) {
    buffer.add(i);
  }
  auto it = buffer.begin();
  buffer.advance(it, 5);
  ASSERT_DOUBLE_EQ(it->first, 5);

  // check advance negative steps
  auto it2 = buffer.end();
  buffer.advance(it2, -5);
  ASSERT_DOUBLE_EQ(it2->first, 95);
  buffer.advance(it2, -1);  // edge case : start at the split point and advance -1
  ASSERT_DOUBLE_EQ(it2->first, 94);
}

TEST(PercentileBufferTest, IteratorTest) {
  PercentileBuffer<double> buffer(10);
  for (int i = 0; i < 10; ++i) {
    buffer.add(i);
  }

  int i = 0;
  for (auto it = buffer.begin(); it != buffer.end(); buffer.next(it)) {
    ASSERT_DOUBLE_EQ(it->first, (double)i);
    ++i;
  }
}

TEST(PercentileBufferTest, NextTest) {
  PercentileBuffer<double> buffer(10);
  for (int i = 0; i < 10; ++i) {
    buffer.add(i);
  }
  auto it = buffer.begin();
  buffer.next(it);
  ASSERT_DOUBLE_EQ(it->first, 1);
}

TEST(PercentileBufferTest, PreviousTest) {
  PercentileBuffer<double> buffer(10);
  for (int i = 0; i < 10; ++i) {
    buffer.add(i);
  }
  auto it = buffer.end();
  buffer.previous(it);
  ASSERT_DOUBLE_EQ(it->first, 9);

  auto it2 = buffer.begin();
  buffer.previous(it2);
  buffer.previous(it2);
  ASSERT_DOUBLE_EQ(it->first, 9);
}

TEST(PercentileBufferTest, EmptyTest) {
  PercentileBuffer<double> buffer(10);
  ASSERT_TRUE(buffer.empty());

  // get percentile when buffer is empty
  ASSERT_EQ(buffer.getPercentile(), 0.0);

  buffer.add(1.0);
  ASSERT_FALSE(buffer.empty());
}

TEST(PercentileBufferTest, FullTest) {
  PercentileBuffer<double> buffer(10);
  for (int i = 0; i < 10; ++i) {
    ASSERT_FALSE(buffer.full());
    buffer.add(i);
  }
  ASSERT_TRUE(buffer.full());
}

TEST(PercentileBufferTest, MaxSizeTest) {
  PercentileBuffer<double> buffer(10);
  ASSERT_EQ(buffer.maxSize(), 10);
}

TEST(PercentileBufferTest, CurrentIndexTest) {
  PercentileBuffer<double> buffer(10);
  for (int i = 0; i < 10; ++i) {
    ASSERT_EQ(buffer.currentIndex(), i);
    buffer.add(i);
  }
}

TEST(PercentileBufferTest, ConstIteratorTest) {
  PercentileBuffer<double> buffer(10);
  for (int i = 0; i < 10; ++i) {
    buffer.add(i);
  }
  const PercentileBuffer<double>& const_buffer = buffer;
  int i = 0;
  for (auto it = const_buffer.begin(); it != const_buffer.end(); const_buffer.next(it)) {
    ASSERT_DOUBLE_EQ(it->first, (double)i);
    ++i;
  }

  // test reverse order
  i = 9;
  auto it = const_buffer.end();
  const_buffer.previous(it);
  for (; it != const_buffer.begin(); const_buffer.previous(it)) {
    ASSERT_DOUBLE_EQ(it->first, (double)i);
    --i;
  }
}

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
