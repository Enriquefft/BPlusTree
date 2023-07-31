#ifndef LEAF_NODE_HPP
#define LEAF_NODE_HPP

#include "Concepts.hpp"

template <size_t M, properKeyValue Key, properKeyValue T,
          std::predicate<Key, Key> Compare, Indexor<Key, T> Indexer,
          IsAllocator Allocator, bool isSet, size_t MAX_CHILDS, size_t MAX_KEYS>
class BPlusTree;

/**
 * @class LeafNode
 * @brief Leaf node for B+ tree.
 * @details The LeafNode class inherits from @ref Node "Node" and adds an
 * array of values and pointers to the next and previous leaf nodes.
 * */
template <size_t M, properKeyValue Key, properKeyValue T,
          std::predicate<Key, Key> Compare, Indexor<Key, T> Indexer,
          IsAllocator Allocator, bool isSet, size_t MAX_CHILDS = M,
          size_t MAX_KEYS = M - 1>
class LeafNode {

  friend class BPlusTree<M, Key, T, Compare, Indexer, Allocator, isSet,
                         MAX_CHILDS, MAX_KEYS>;

private:
  using value_type = std::conditional_t<isSet, Key, std::pair<Key, T>>;

  std::array<value_type *, MAX_KEYS>
      m_values;               ///< Array of (M-1) values_types (key-value pairs)
  LeafNode *m_next = nullptr; ///< Pointer to next leaf node
  LeafNode *m_prev = nullptr; ///< Pointer to previous leaf node
};

#endif // !LEAF_NODE_HPP
