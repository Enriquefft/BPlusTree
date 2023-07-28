#include <gtest/gtest.h>
#include <type_traits>

#define NO_TEMPLATE_TESTS

#ifndef NO_TEMPLATE_TESTS

#include "BPlusTree.hpp"

TEST(TemplateParametersTesting, OnlyKey) {
  using key_type = int;
  BPlusTree<4, key_type> tree;

  ASSERT_TRUE((std::is_same_v<decltype(tree)::key_type, key_type>));
}

TEST(TemplateParametersTesting, KeyAndValue) {

  using key_type = int;
  using value_type = int;
  BPlusTree<4, key_type, value_type> tree;
  ASSERT_TRUE((std::is_same_v<decltype(tree)::key_type, key_type>));
  ASSERT_TRUE((std::is_same_v<decltype(tree)::mapped_type, value_type>));
}

class FloatCompare {
public:
  bool operator()(const float &a, const float &b) const { return a < b; }
};

TEST(TemplateParametersTesting, KeyValueAndCompare) {

  using key_type = int;
  BPlusTree<4, key_type, FloatCompare> tree;

  ASSERT_TRUE((std::is_same_v<decltype(tree)::key_type, key_type>));

  ASSERT_FALSE(
      (std::is_same_v<decltype(tree)::key_compare, std::less<key_type>>));
  ASSERT_TRUE((std::is_same_v<decltype(tree)::key_compare, FloatCompare>));
}

#endif
