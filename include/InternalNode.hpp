#ifndef INTERNAL_NODE_HPP
#define INTERNAL_NODE_HPP

#include "Concepts.hpp"

template <BPLUS_TEMPLATES, size_t MAX_CHILDS, size_t MAX_KEYS>
class NodeHandler;

/**
 * @class InternalNode
 * @brief Internal node for B+ tree.
 * @details The InternalNode class inherits from @ref Node "Node" and adds an
 * array of pointers to child nodes.
 * */
template <BPLUS_TEMPLATES, size_t MAX_CHILDS = M, size_t MAX_KEYS = M - 1>
class InternalNode {

  friend class BPlusTree<BPLUS_TEMPLATE_PARAMS>;

private:
  using NodeHandler_ = NodeHandler<BPLUS_TEMPLATE_PARAMS, MAX_CHILDS, MAX_KEYS>;

  std::array<Key, MAX_KEYS> m_keys;                  ///< Array of (M-1) keys
  std::array<NodeHandler_ *, MAX_CHILDS> m_children; ///< Array of M children
};

#endif // !INTERNAL_NODE_HPP
