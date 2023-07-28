#include <gtest/gtest.h>

#include "../include/BPlusTree.hpp"

// Generate BPlusTrees

namespace {
template <typename Key, typename Value>
BPlusTree<3, Key, Value> getSimpleMap() {
  return {};
}
} // namespace

TEST(insert, InsertUniqValue) {
  auto map = getSimpleMap<int, std::string>();
  auto [iter, inserted] = map.insert(std::make_pair(1, "value1"));

  // Check if the value is inserted
  ASSERT_TRUE(inserted);

  // Check if the iterator points to the inserted value
  // TODO(enrique): Uncomment the following line after implementing the iterator
  // ASSERT_EQ(iter->first, 1);
  // ASSERT_EQ(iter->second, "value1");
}

TEST(insert, InsertDuplicateValue) {
  auto map = getSimpleMap<int, std::string>();
  map.insert(std::make_pair(1, "value1"));
  auto [iter, inserted] = map.insert(std::make_pair(1, "value1"));

  // Check if the value is not inserted
  ASSERT_FALSE(inserted);
}

TEST(insert, InsertInvalidValue) {
  auto map = getSimpleMap<int, std::string>();

  // Trying to insert invalid/null value
  EXPECT_THROW(map.insert(std::make_pair(1, NULL)), std::invalid_argument);
}

TEST(insert, InsertOrder) {
  auto map = getSimpleMap<int, std::string>();
  map.insert(std::make_pair(3, "value3"));
  map.insert(std::make_pair(1, "value1"));
  map.insert(std::make_pair(2, "value2"));

  // TODO(enrique): Uncomment the following lines after implementing the
  // iterator

  // auto iter = map.begin();
  // ASSERT_EQ(iter->first, 1);
  // ++iter;
  // ASSERT_EQ(iter->first, 2);
  // ++iter;
  // ASSERT_EQ(iter->first, 3);
}
