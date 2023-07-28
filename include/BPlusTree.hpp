#ifndef BPlusTree_HPP
#define BPlusTree_HPP

#include "Concepts.hpp"
#include <bits/iterator_concepts.h>
#include <functional>

constexpr size_t MIN_DEGREE = 3;

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
          class Allocator = std::allocator<const Key>, //
          size_t CHILD_COUNT = M, size_t KEY_COUNT = M - 1>
class BPlusTree {

  // M must be at least 3
  static_assert(M >= MIN_DEGREE, "M(B+Tree degree) must be at least 3");

  // T must not be functor
  static_assert(
      NonFunctor<T>,
      "T must not be a functor or predicate, this issue proably means that the "
      "predicate wasn't detected by sfinae due to wrong template parameters.\n "
      "Eg. operator()(int, int) When Key is float");

  // potential missuse of CHILD_COUNT and KEY_COUNT
  static_assert(CHILD_COUNT == M,
                "CHILD_COUNT must not be manually changed\nIf you want to "
                "change the order of the "
                "tree, change M\nCHILD_COUNT is used for internal purposes");
  static_assert(KEY_COUNT == M - 1,
                "KEY_COUNT must not be manually changed\nIf you want to change "
                "the order of "
                "the tree, change M\nKEY_COUNT is used for internal purposes");

  // Indexor asserts
  static_assert(!(std::is_same<Indexer, Identity>::value &&
                  !std::is_same<Key, T>::value),
                "If Indexor is Identity, then Key and T must be the same");

  // if Indexor is not Identity, then it must be a functor
  static_assert(
      std::is_same<Indexer, Identity>::value || is_indexer<Indexer, T, Key>,
      "Indexor must be a functor or function\nIf you want to use Identity, "
      "then Key and T must be the same");

  template <bool isConst> class BPlusTreeIterator;
  class Node;
  class LeafNode;
  class InternalNode;

public:
  /// @defgroup TypeDefinitions Type Definitions
  /// @brief Provides typedefs used throughout the BPlusTree class
  /// @{

  /// @brief Type definition for representing keys in BPlusTree
  using key_type = Key;

  /// @brief Type definition for representing mapped or associated values in
  /// BPlusTree
  using mapped_type = T;

  /// @brief Type definition for representing value pairs in BPlusTree
  using value_type = std::pair<const Key, T>;

  /// @brief Type definition for representing size in BPlusTree
  using size_type = size_t;

  /// @brief Type definition for difference type in BPlusTree
  using difference_type = std::ptrdiff_t;

  /// @brief Type definition for key comparison function object in BPlusTree
  using key_compare = Compare;

  /// @brief Type definition for node allocator in BPlusTree
  using NodeAllocator = typename Allocator::template rebind<Node>::other;

  /// @brief Type definition for leaf node allocator in BPlusTree
  using LeafNodeAllocator =
      typename Allocator::template rebind<LeafNode>::other;

  /// @brief Type definition for internal node allocator in BPlusTree
  using InternalNodeAllocator =
      typename Allocator::template rebind<InternalNode>::other;

  /// @brief Type definition for reference to the value_type
  using reference = value_type &;

  /// @brief Type definition for constant reference to the value_type
  using const_reference = const value_type &;

  /// @brief Type definition for pointer to the element in BPlusTree
  using pointer = typename std::allocator_traits<Allocator>::pointer;

  /// @brief Type definition for constant pointer to the element in BPlusTree
  using const_pointer =
      typename std::allocator_traits<Allocator>::const_pointer;

  /// @brief Type definition for BPlusTree iterator
  using iterator = BPlusTreeIterator<false>;

  /// @brief Type definition for constant BPlusTree iterator
  using const_iterator = BPlusTreeIterator<true>;

  /// @brief Type definition for reverse BPlusTree iterator
  using reverse_iterator = std::reverse_iterator<iterator>;

  /// @brief Type definition for constant reverse BPlusTree iterator
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;

  /// @brief Type definition for indexor, used for testing purposes
  using indexor = Indexer;

  /// @}

  /// @defgroup Constructors B+Tree Constructors
  /// @name Constructors
  /// @brief B+Tree constructors
  /// @details These constructors allow for any combination of comp, alloc and
  /// indexor to be configured at construction time.

  /// @{
  /// @brief Default constructor
  /// @details It uses default comparator, allocator and indexer.
  BPlusTree() : BPlusTree(Compare()) {}

  /// @brief Comprehensive constructor
  /// @details It allows configuration of comparator, allocator and indexer.
  /// @param comp Comparator configuration.
  /// @param alloc Allocator configuration. Defaults to Allocator().
  /// @param indexor Indexer configuration. Defaults to Indexer().
  explicit BPlusTree(const Compare &comp, const Allocator &alloc = Allocator(),
                     const Indexer &indexor = Indexer());

  /// @brief Allocator-based constructor
  /// @details It allows configuration of allocator and indexer.
  /// @param alloc Allocator configuration.
  /// @param indexor Indexer configuration. Defaults to Indexer().
  explicit BPlusTree(const Allocator &alloc,
                     const Indexer &indexor = Indexer());

  /// @brief Comparator & Indexer-based constructor
  /// @details It allows configuration of comparator and indexer.
  /// @param comp Comparator configuration.
  /// @param indexor Indexer configuration.
  explicit BPlusTree(const Compare &comp, const Indexer &indexor);

  /// @brief Indexer-based constructor
  /// @details It allows configuration of only indexer.
  /// @param indexor Indexer configuration.
  explicit BPlusTree(const Indexer &indexor);

  /// @brief Iterator-based constructor
  /// @details Allows input of range and configuration of comparator, allocator
  /// and indexer.
  /// @param [in] first Iterator pointing to the start of input range.
  /// @param [in] last Iterator pointing to the end of input range.
  /// @param comp Comparator.
  /// @param alloc Allocator. Defaults to Allocator().
  /// @param indexor Indexer. Defaults to Indexer().
  template <std::input_iterator InputIt>
  BPlusTree(InputIt first, InputIt last, const Compare &comp,
            const Allocator &alloc = Allocator(),
            const Indexer &indexor = Indexer());

  /// @brief Iterator and Allocator-based constructor
  /// @details Allows input of range and configuration of allocator and indexer.
  /// @param [in] first Iterator pointing to the start of input range.
  /// @param [in] last Iterator pointing to the end of input range.
  /// @param alloc Allocator.
  /// @param indexor Indexer. Defaults to Indexer().
  template <std::input_iterator InputIt>
  BPlusTree(InputIt first, InputIt last, const Allocator &alloc,
            const Indexer &indexor = Indexer()) {}

  /// @brief Iterator and Comparator & Indexer-based constructor
  /// @details Allows input of range and configuration of comparator and
  /// indexer.
  /// @param [in] first Iterator pointing to the start of input range.
  /// @param [in] last Iterator pointing to the end of input range.
  /// @param comp Comparator.
  /// @param indexor Indexer.
  template <std::input_iterator InputIt>
  BPlusTree(InputIt first, InputIt last, const Compare &comp,
            const Indexer &indexor) {}

  /// @brief Iterator and Indexer-based constructor
  /// @details Allows input of range and configuration of indexer.
  /// @param [in] first Iterator pointing to the start of input range.
  /// @param [in] last Iterator pointing to the end of input range.
  /// @param indexor Indexer.
  template <std::input_iterator InputIt>
  BPlusTree(InputIt first, InputIt last, const Indexer &indexor) {}

  /// @brief Copy constructor
  /// @details Constructs a new object as a copy of an existing one
  /// @param other Another object to be used as source to initialize elements of
  /// the container with.
  BPlusTree(const BPlusTree &other);

  /// @brief Allocator-aware copy constructor
  /// @details Constructs a new object as a copy of an existing one using
  /// specified allocator
  /// @param other Another object to be used as source to initialize elements of
  /// the container with.
  /// @param alloc Allocator.
  BPlusTree(const BPlusTree &other, const Allocator &alloc);

  /// @brief Move constructor
  /// @details Constructs a new object that is a materialization of an existing,
  /// expiring object
  /// @param other Another object (rvalue) to be used as source to initialize
  /// elements of the container with.
  BPlusTree(BPlusTree &&other) noexcept;

  /// @brief Allocator-aware move constructor
  /// @details Constructs a new object that is a materialization of an existing,
  /// expiring object with specified allocator
  /// @param other Another object (rvalue) to be used as source to initialize
  /// elements of the container with.
  /// @param alloc Allocator.
  BPlusTree(BPlusTree &&other, const Allocator &alloc);

  /// @brief Move assignment operator
  /// @details Replaces the contents with those of other using move semantics
  /// (i.e., the data in other is moved from other into this container).
  /// @param other Another object to be used as source to initialize elements of
  /// the container with.
  BPlusTree &operator=(BPlusTree &&other) noexcept;

  /// @brief Copy assignment operator.
  /// @details Replaces the contents with a copy of the contents of other.
  /// @param other Another object to be used as source to initialize elements of
  /// the container with.
  BPlusTree &operator=(const BPlusTree &other);

  /// @brief Initializer list assignment operator
  /// @details Replaces the contents with the elements from the initializer list
  /// ilist.
  /// @param ilist Initializer list to assign from.
  BPlusTree &operator=(std::initializer_list<value_type> ilist);

  /// @brief Initializer list constructor with compare and alloc
  /// @details Constructs a new object from the given initializer list
  /// @param init Initializer list to assign from.
  /// @param comp Comparator.
  /// @param alloc Allocator.
  BPlusTree(std::initializer_list<value_type> init,
            const Compare &comp = Compare(),
            const Allocator &alloc = Allocator());

  /// @brief Initializer list constructor with allocator
  /// @details Constructs a new object from the given initializer list with
  /// specified allocator
  /// @param init Initializer list to assign from.
  /// @param alloc Allocator.
  BPlusTree(std::initializer_list<value_type> init, const Allocator &alloc)
      : BPlusTree(init, Compare(), alloc) {}

  /// @}

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
    std::array<T, KEY_COUNT> values; ///< Array of (M-1) values
    LeafNode *next;                  ///< Pointer to next leaf node
    LeafNode *prev;                  ///< Pointer to previous leaf node
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
  Node *root = nullptr;
  NodeAllocator nodeAlloc;
  LeafNodeAllocator leafNodeAlloc;
  InternalNodeAllocator internalNodeAlloc;
};

template <size_t M, properKeyValue Key, std::predicate<Key, Key> Compare,
          typename Indexer>
  requires(M >= MIN_DEGREE)
class BPlusTree<M, Key, Compare, Indexer>
    : public BPlusTree<M, Key, Key, Compare, Indexer> {};

template <size_t M, properKeyValue Key, std::predicate<Key, Key> Compare>
  requires(M >= MIN_DEGREE)
class BPlusTree<M, Key, Compare> : public BPlusTree<M, Key, Key, Compare> {};

#endif // !BPlusTree_HPP
