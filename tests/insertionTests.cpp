#include <gtest/gtest.h>

#include "Map.hpp"
#include "Set.hpp"

template <typename Key, typename Value> Map<3, Key, Value> getSimpleMap() {
  auto map = Map<3, Key, Value>{};
  return map;
}

TEST(BPlusTreeTest, InsertionTest1) {
  auto tree = getSimpleMap<int, int>();
  // is map
  ASSERT_EQ(tree.is_map(), false);

  // auto pair = tree.insert({1, 1});

  // ASSERT_TRUE(pair.second);

  // TODO (enrique): Uncomment the following lines after implementing the
  // iterator
  // ASSERT_EQ(pair.first->first, 1);
  // ASSERT_EQ(pair.first->second, 1);
}

// TEST(BPlusTreeTest, InsertionTest_Rvalue1) {
//   auto tree = Set<3, int>();
//
//   // auto pair = tree.insert(1);
//
//   // ASSERT_TRUE(pair.second);
//   // ASSERT_EQ(pair.first->first, 1);
//   // ASSERT_EQ(pair.first->second, 1);
// }
//
// TEST(BPlusTreeTest, InsertionTest_Position1) {
//   Map<3, int, int> tree;
//
//   // tree.insert({1, 1});
//   // auto it = tree.insert(tree.begin(), {2, 2});
//   //
//   // ASSERT_EQ(it->first, 2);
//   // ASSERT_EQ(it->second, 2);
// }
//
// TEST(BPlusTreeTest, InsertionTest_Position_Rvalue1) {
//   Map<3, int, int> tree;
//
//   // tree.insert({1, 1});
//   // auto it = tree.insert(tree.begin(), {2, 2});
//   //
//   // ASSERT_EQ(it->first, 2);
//   // ASSERT_EQ(it->second, 2);
// }
//
// TEST(BPlusTreeTest, InsertionTest_Range1) {
//   Map<3, int, int> tree;
//
//   // std::initializer_list<std::pair<const int, int>> list = {{1, 1}, {2,
//   2}};
//   // tree.insert(begin(list), end(list));
//   //
//   // auto it = tree.find(1);
//   //
//   // ASSERT_NE(it, tree.end());
//   // ASSERT_EQ(it->first, 1);
//   // ASSERT_EQ(it->second, 1);
//
//   // it = tree.find(2);
//   //
//   // ASSERT_NE(it, tree.end());
//   // ASSERT_EQ(it->first, 2);
//   // ASSERT_EQ(it->second, 2);
// }
//
// TEST(BPlusTreeTest, InsertionTest_InitializerList1) {
//   Map<3, int, int> tree;
//
//   // tree.insert({{1, 1}, {2, 2}});
//   //
//   // auto it = tree.find(1);
//   //
//   // ASSERT_NE(it, tree.end());
//   // ASSERT_EQ(it->first, 1);
//   // ASSERT_EQ(it->second, 1);
//   //
//   // it = tree.find(2);
//   //
//   // ASSERT_NE(it, tree.end());
//   // ASSERT_EQ(it->first, 2);
//   // ASSERT_EQ(it->second, 2);
// }
//
// TEST(insert, InsertUniqValue) {
//   auto map = Map<3, int, std::string>();
//   // auto [iter, inserted] = map.insert(std::make_pair(1, "value1"));
//
//   // Check if the value is inserted
//   // ASSERT_TRUE(inserted);
//
//   // Check if the iterator points to the inserted value
//   // TODO(enrique): Uncomment the following line after implementing the
//   iterator
//   // ASSERT_EQ(iter->first, 1);
//   // ASSERT_EQ(iter->second, "value1");
// }
//
// TEST(insert, InsertDuplicateValue) {
//   auto map = Map<3, int, std::string>();
//   // map.insert(std::make_pair(1, "value1"));
//   // auto [iter, inserted] = map.insert(std::make_pair(1, "value1"));
//
//   // Check if the value is not inserted
//   // ASSERT_FALSE(inserted);
// }
//
// TEST(insert, InsertOrder) {
//   auto map = Map<3, int, std::string>();
//   // map.insert(std::make_pair(3, "value3"));
//   // map.insert(std::make_pair(1, "value1"));
//   // map.insert(std::make_pair(2, "value2"));
//
//   // TODO(enrique): Uncomment the following lines after implementing the
//   // iterator
//
//   // auto iter = map.begin();
//   // ASSERT_EQ(iter->first, 1);
//   // ++iter;
//   // ASSERT_EQ(iter->first, 2);
//   // ++iter;
//   // ASSERT_EQ(iter->first, 3);
// }
