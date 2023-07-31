#ifndef INTERNAL_NODE_HPP
#define INTERNAL_NODE_HPP

#include "Concepts.hpp"
template <size_t M, properKeyValue Key, properKeyValue T,
          std::predicate<Key, Key> Compare, IsAllocator Allocator, bool isSet,
          size_t MAX_CHILDS, size_t MAX_KEYS>
class BPlusTree;

template <size_t M, properKeyValue Key, properKeyValue T,
          std::predicate<Key, Key> Compare, IsAllocator Allocator, bool isSet,
          size_t MAX_CHILDS, size_t MAX_KEYS>
class NodeHandler;

/**
 * @class InternalNode
 * @brief Internal node for B+ tree.
 * @details The InternalNode class inherits from @ref Node "Node" and adds an
 * array of pointers to child nodes.
 * */
template <size_t M, properKeyValue Key, properKeyValue T,
          std::predicate<Key, Key> Compare, IsAllocator Allocator, bool isSet,
          size_t MAX_CHILDS = M, size_t MAX_KEYS = M - 1>
class InternalNode {
  friend class BPlusTree<M, Key, T, Compare, Allocator, isSet, MAX_CHILDS,
                         MAX_KEYS>;

private:
  using NodeHandler_ =
      NodeHandler<M, Key, T, Compare, Allocator, isSet, MAX_CHILDS, MAX_KEYS>;
  std::array<Key, MAX_KEYS> m_keys;                  ///< Array of (M-1) keys
  std::array<NodeHandler_ *, MAX_CHILDS> m_children; ///< Array of M children
};

#endif // !INTERNAL_NODE_HPP
