#include <gtest/gtest.h>

#include "toolbox/lrucache.h"

TEST(TestLRUCache, AddRemoveQuery) {
  LRUCache<int, char> cache{4};

  EXPECT_TRUE(cache.empty());
  EXPECT_FALSE(cache.full());
  EXPECT_EQ(0, cache.size());
  EXPECT_FALSE(cache.present(1));
  EXPECT_EQ(nullptr, cache.query(1));

  cache.insert(1, 1);
  EXPECT_FALSE(cache.empty());
  EXPECT_FALSE(cache.full());
  EXPECT_EQ(1, cache.size());
  EXPECT_TRUE(cache.present(1));
  ASSERT_NE(nullptr, cache.query(1));
  EXPECT_EQ(1, *cache.query(1));

  cache.insert(2, 2);
  EXPECT_EQ(2, cache.size());
  EXPECT_FALSE(cache.full());
  ASSERT_NE(nullptr, cache.query(1));
  ASSERT_NE(nullptr, cache.query(2));
  EXPECT_EQ(1, *cache.query(1));
  EXPECT_EQ(2, *cache.query(2));

  cache.insert(3, 3);
  EXPECT_EQ(3, cache.size());
  EXPECT_FALSE(cache.full());
  ASSERT_NE(nullptr, cache.query(1));
  ASSERT_NE(nullptr, cache.query(2));
  ASSERT_NE(nullptr, cache.query(3));
  EXPECT_EQ(1, *cache.query(1));
  EXPECT_EQ(2, *cache.query(2));
  EXPECT_EQ(3, *cache.query(3));

  cache.insert(4, 4);
  EXPECT_EQ(4, cache.size());
  EXPECT_TRUE(cache.full());
  ASSERT_NE(nullptr, cache.query(1));
  ASSERT_NE(nullptr, cache.query(2));
  ASSERT_NE(nullptr, cache.query(3));
  ASSERT_NE(nullptr, cache.query(4));
  EXPECT_EQ(1, *cache.query(1));
  EXPECT_EQ(2, *cache.query(2));
  EXPECT_EQ(3, *cache.query(3));
  EXPECT_EQ(4, *cache.query(4));
  EXPECT_EQ(1, *cache.toEvict());

  cache.insert(5, 5);
  EXPECT_EQ(4, cache.size());
  EXPECT_TRUE(cache.full());
  EXPECT_FALSE(cache.present(1));
  EXPECT_TRUE(cache.present(2));
  EXPECT_TRUE(cache.present(3));
  EXPECT_TRUE(cache.present(4));
  EXPECT_TRUE(cache.present(5));
  EXPECT_EQ(nullptr, cache.query(1));
  ASSERT_NE(nullptr, cache.query(2));
  ASSERT_NE(nullptr, cache.query(3));
  ASSERT_NE(nullptr, cache.query(4));
  ASSERT_NE(nullptr, cache.query(5));
  EXPECT_EQ(2, *cache.query(2));
  EXPECT_EQ(3, *cache.query(3));
  EXPECT_EQ(4, *cache.query(4));
  EXPECT_EQ(5, *cache.query(5));
  EXPECT_EQ(2, *cache.toEvict());
}

TEST(TestLRUCache, Resize) {
  LRUCache<int, char> cache{4};

  cache.insert(1, 1);
  cache.insert(2, 2);
  cache.insert(3, 3);
  cache.insert(4, 4);
  EXPECT_EQ(4, cache.size());
  EXPECT_TRUE(cache.full());

  cache.resize(4);
  EXPECT_EQ(4, cache.size());
  EXPECT_TRUE(cache.full());
  EXPECT_TRUE(cache.present(1));
  EXPECT_TRUE(cache.present(2));
  EXPECT_TRUE(cache.present(3));
  EXPECT_TRUE(cache.present(4));

  cache.resize(5);
  EXPECT_EQ(4, cache.size());
  EXPECT_FALSE(cache.full());
  cache.insert(5, 5);
  EXPECT_EQ(5, cache.size());
  EXPECT_TRUE(cache.full());
  EXPECT_TRUE(cache.present(1));
  EXPECT_TRUE(cache.present(2));
  EXPECT_TRUE(cache.present(3));
  EXPECT_TRUE(cache.present(4));
  EXPECT_TRUE(cache.present(5));

  cache.resize(4);
  EXPECT_EQ(4, cache.size());
  EXPECT_TRUE(cache.full());
  EXPECT_FALSE(cache.present(1));
  EXPECT_TRUE(cache.present(2));
  EXPECT_TRUE(cache.present(3));
  EXPECT_TRUE(cache.present(4));
  EXPECT_TRUE(cache.present(5));
}

int main(int argc, char** argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
