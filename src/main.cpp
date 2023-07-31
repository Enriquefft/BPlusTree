#include "Map.hpp"
#include "Set.hpp"
#include <iostream>

int main() {

  auto tree = Map<3, int, int>();
  // is map

  auto succes = tree.insert({1, 1});

  // BPlusTree<4, int> tree1; // M = 4, Key = int, T = int, Indexer = Identity
  // BPlusTree<3, float, FloatCompare>
  //     tree2; // M = 3, Key = float, T = float, Compare = FloatCompare,
  //     Indexer =
  //            // Identity

  std::cout << "Hello, World!\n";
  return 0;
}
