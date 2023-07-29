#include "Concepts.hpp"
#include <functional>

/**
 * @class BPlusTree
 * @brief Generic B+ Tree class
 * @tparam M Order of the tree
 * @tparam Key Key type
 * @tparam T Value type
 * @tparam Compare Comparison function
 * @tparam Allocator Allocator type
 *
 * @details
 * B+ Tree is a self-balancing tree data structure that keeps data sorted and
 * allows searches, sequential access, insertions, and deletions in logarithmic
 * time.
 *
 * */
template <size_t M, properKeyValue Key, properKeyValue T = Key,
          std::predicate<Key, Key> Compare = std::less<Key>,
          typename Indexer = Identity,
          class Allocator = std::allocator<std::pair<const Key, T>>, //
          size_t CHILD_COUNT = M, size_t KEY_COUNT = M - 1>
class BPlusTree {

  using isSet =
      std::integral_constant<bool, std::is_same<Indexer, Identity>::value &&
                                       std::is_same<Key, T>::value>;
  using value_type =
      std::conditional_t<isSet::value, Key, std::pair<const Key, T>>;
  using allocator_type = Allocator;
  using key_compare = Compare;

private:
  // Internal use classes

  /**
   * @class Node
   * @brief Base Node structure for B+ tree.
   * @details The Node structure consists of an array of keys and a flag for
   * whether the node is a leaf. It is the base class for both @ref LeafNode
   * "LeafNode" and @ref InternalNode "InternalNode".
   */
  class Node {
    std::array<Key, KEY_COUNT> keys; ///< Array of (M-1) keys
    /**
     * @brief Boolean flag to know whether the node is a leaf.
     *
     * Used to maintain:
     * https://en.wikipedia.org/wiki/Liskov_substitution_principle
     */
    bool isLeaf;
  };

  /**
   * @class LeafNode
   * @brief Leaf node for B+ tree.
   * @details The LeafNode class inherits from @ref Node "Node" and adds an
   * array of values and pointers to the next and previous leaf nodes.
   * */
  class LeafNode : public Node {
    std::array<value_type, KEY_COUNT>
        values;     ///< Array of (M-1) values_types (key-value pairs)
    LeafNode *next; ///< Pointer to next leaf node
    LeafNode *prev; ///< Pointer to previous leaf node
  };

  /**
   * @class InternalNode
   * @brief Internal node for B+ tree.
   * @details The InternalNode class inherits from @ref Node "Node" and adds an
   * array of pointers to child nodes.
   * */
  class InternalNode : public Node {
    std::array<Node *, CHILD_COUNT> children; ///< Array of M children
  };

  /**
   * @class BPlusTreeIterator
   * @brief Iterator for B+ tree.
   * @details The BPlusTreeIterator class is a bidirectional iterator which
   * follows the standard
   * */
  template <bool isConst> class BPlusTreeIterator {};

  // Private members
  Node *m_root = nullptr;
  allocator_type m_allocator;
  key_compare m_comp;
  Indexer m_indexor;
};
