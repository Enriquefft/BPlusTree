#ifndef NODE_HANDLER_HPP
#define NODE_HANDLER_HPP

#include "array"
#include <variant>

#include "Concepts.hpp"
#include "InternalNode.hpp"
#include "LeafNode.hpp"

template <typename T> struct erase_parenthesis;
template <typename T, typename U> struct erase_parenthesis<T(U)> {
  using type = U;
};

// Macro to define methods based on which node type can use them
#define ONLY_INTERNAL(RETURN_TYPE, NAME, ARGUMENTS, CALLER_ARGS)               \
  typename erase_parenthesis<void RETURN_TYPE>::type NAME ARGUMENTS {          \
    if (auto *node_ptr = std::get_if<InternalNode *>(&m_node)) {               \
      return node_ptr->NAME CALLER_ARGS;                                       \
    }                                                                          \
    throw std::runtime_error("Cant " #NAME " in non inner node");              \
  }

#define ONLY_LEAF(RETURN_TYPE, NAME, ARGUMENTS, CALLER_ARGS)                   \
  typename erase_parenthesis<void RETURN_TYPE>::type NAME ARGUMENTS {          \
    if (auto *node_ptr = std::get_if<LeafNode *>(&m_node)) {                   \
      return node_ptr->NAME CALLER_ARGS;                                       \
    }                                                                          \
    throw std::runtime_error("Cant " #NAME " in non inner node");              \
  }

/**
 * @class NodeHandler
 * @brief Base Node structure for B+ tree.
 * @details The Node structure consists of an array of keys and a flag for
 * whether the node is a leaf. It is the base class for both @ref LeafNode
 * "LeafNode" and @ref InternalNode "InternalNode".
 */
template <size_t M, properKeyValue Key, properKeyValue T,
          std::predicate<Key, Key> Compare, IsAllocator Allocator, bool isSet,
          size_t MAX_CHILDS = M, size_t MAX_KEYS = M - 1>
class NodeHandler {

  friend class BPlusTree<M, Key, T, Compare, Allocator, isSet, MAX_CHILDS,
                         MAX_KEYS>;

  using value_type = std::conditional_t<isSet, Key, std::pair<const Key, T>>;

  using LeafNode =
      LeafNode<M, Key, T, Compare, Allocator, isSet, MAX_CHILDS, MAX_KEYS>;
  using InternalNode =
      InternalNode<M, Key, T, Compare, Allocator, isSet, MAX_CHILDS, MAX_KEYS>;
  using iterator = BPlusTreeIterator<false, M, Key, T, Compare, Allocator,
                                     isSet, MAX_CHILDS, MAX_KEYS>;
  using const_iterator = BPlusTreeIterator<true, M, Key, T, Compare, Allocator,
                                           isSet, MAX_CHILDS, MAX_KEYS>;

  NodeHandler(LeafNode *leaf_node) : m_node(leaf_node), m_isLeaf(true) {}
  NodeHandler(InternalNode *internal_node)
      : m_node(internal_node), m_isLeaf(false) {}
  NodeHandler(std::nullptr_t) : m_node(nullptr), m_isLeaf(false) {}

  NodeHandler &operator=(const LeafNode *&leaf_node) {
    m_node = leaf_node;
    m_isLeaf = true;
  }
  NodeHandler &operator=(const InternalNode *&internal_node) {
    m_node = internal_node;
    m_isLeaf = false;
  }
  NodeHandler &operator=(std::nullptr_t) {
    m_node = nullptr;
    m_isLeaf = false;
  }

  // spaceship
  [[nodiscard]] constexpr auto operator<=>(const NodeHandler &) const = default;

  ONLY_LEAF((std::array<value_type *, MAX_KEYS> &), values, (), ())
  ONLY_INTERNAL((std::array<Key, MAX_KEYS> &), keys, (), ())

  std::array<NodeHandler *, MAX_CHILDS> &childs() {
    if (auto *node_ptr = std::get_if<InternalNode *>(&m_node)) {
      return node_ptr->m_childs;
    }
    throw std::runtime_error("Cant get childs from non internal node");
  }

  LeafNode *&next() {
    if (auto *node_ptr = std::get_if<LeafNode *>(&m_node)) {
      return node_ptr->m_next;
    }
    throw std::runtime_error("Cant get next from non leaf node");
  }
  LeafNode *&prev() {
    if (auto *node_ptr = std::get_if<LeafNode *>(&m_node)) {
      return node_ptr->m_prev;
    }
    throw std::runtime_error("Cant get prev from non leaf node");
  }
  std::pair<iterator, InsertResult> insert(const value_type &value) {
    if (auto *node_ptr = std::get_if<LeafNode *>(&m_node)) {
      return (*node_ptr)->insert(value);
    }
    throw std::runtime_error("Cant insert in non leaf node");
  }

  std::variant<std::nullptr_t, LeafNode *, InternalNode *> m_node;
  /**
   * @brief Boolean flag to know whether the node is a leaf.
   * Used to maintain:
   * https://en.wikipedia.org/wiki/Liskov_substitution_principle
   */
  bool m_isLeaf;
};

#endif // !NODE_HANDLER_HPP
