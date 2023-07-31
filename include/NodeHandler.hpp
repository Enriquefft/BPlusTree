#ifndef NODE_HANDLER_HPP
#define NODE_HANDLER_HPP

#include <variant>

#include "Concepts.hpp"
#include "InternalNode.hpp"
#include "LeafNode.hpp"

/**
 * @class NodeHandler
 * @brief Base Node structure for B+ tree.
 * @details The Node structure consists of an array of keys and a flag for
 * whether the node is a leaf. It is the base class for both @ref LeafNode
 * "LeafNode" and @ref InternalNode "InternalNode".
 */
template <size_t M, properKeyValue Key, properKeyValue T,
          std::predicate<Key, Key> Compare, Indexor<Key, T> Indexer,
          IsAllocator Allocator, bool isSet, size_t MAX_CHILDS = M,
          size_t MAX_KEYS = M - 1>
class NodeHandler {

  friend class BPlusTree<M, Key, T, Compare, Indexer, Allocator, isSet,
                         MAX_CHILDS, MAX_KEYS>;

  using value_type = std::conditional_t<isSet, Key, std::pair<Key, T>>;

  using LeafNode = LeafNode<M, Key, T, Compare, Indexer, Allocator, isSet,
                            MAX_CHILDS, MAX_KEYS>;
  using InternalNode = InternalNode<M, Key, T, Compare, Indexer, Allocator,
                                    isSet, MAX_CHILDS, MAX_KEYS>;

  explicit NodeHandler(LeafNode leaf_node)
      : m_node(leaf_node), m_isLeaf(true) {}
  explicit NodeHandler(InternalNode internal_node)
      : m_node(internal_node), m_isLeaf(false) {}

  NodeHandler &operator=(const LeafNode &leaf_node) {
    m_node = leaf_node;
    m_isLeaf = true;
  }
  NodeHandler &operator=(const InternalNode &internal_node) {
    m_node = internal_node;
    m_isLeaf = false;
  }

  std::array<value_type *, MAX_KEYS> &values() {
    if (auto *node_ptr = std::get_if<LeafNode>(&m_node)) {
      return node_ptr->m_values;
    }
    throw std::runtime_error("Cant get values from non leaf node");
  }
  std::array<Key, MAX_KEYS> &keys() {
    if (auto *node_ptr = std::get_if<InternalNode>(&m_node)) {
      return node_ptr->m_keys;
    }
    throw std::runtime_error("Cant get keys from non internal node");
  }
  std::array<NodeHandler *, MAX_CHILDS> &childs() {
    if (auto *node_ptr = std::get_if<InternalNode>(&m_node)) {
      return node_ptr->m_childs;
    }
    throw std::runtime_error("Cant get childs from non internal node");
  }

  LeafNode *&next() {
    if (auto *node_ptr = std::get_if<LeafNode>(&m_node)) {
      return node_ptr->m_next;
    }
    throw std::runtime_error("Cant get next from non leaf node");
  }
  LeafNode *&prev() {
    if (auto *node_ptr = std::get_if<LeafNode>(&m_node)) {
      return node_ptr->m_prev;
    }
    throw std::runtime_error("Cant get prev from non leaf node");
  }

  std::variant<LeafNode, InternalNode> m_node;
  /**
   * @brief Boolean flag to know whether the node is a leaf.
   * Used to maintain:
   * https://en.wikipedia.org/wiki/Liskov_substitution_principle
   */
  bool m_isLeaf;
};

#endif // !NODE_HANDLER_HPP
