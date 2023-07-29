#ifndef BPlusTree_HPP
#define BPlusTree_HPP

#include "Concepts.hpp"
#include <functional>
#include <memory>

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
          class Allocator = std::allocator<std::pair<const Key, T>>, //
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
  // static_assert(!(std::is_same<Indexer, Identity>::value &&
  //                 !std::is_same<Key, T>::value),
  //               "If Indexor is Identity, then Key and T must be the same");

  // if Key == T, then Indexor must be Identity
  static_assert(std::is_same<Indexer, Identity>::value ||
                    !std::is_same<Key, T>::value,
                "If Key and T are the same, then Indexor must be Identity");

  using isSet =
      std::integral_constant<bool, std::is_same<Indexer, Identity>::value &&
                                       std::is_same<Key, T>::value>;

  // if Indexor is not Identity, then it must be a functor
  static_assert(
      std::is_same<Indexer, Identity>::value || is_indexer<Indexer, T, Key>,
      "Indexor must be a functor or function\nIf you want to use Identity, "
      "then Key and T must be the same");

  template <bool isConst> class BPlusTreeIterator;
  class Node;
  class LeafNode;
  class InternalNode;

  class node_return;

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
  using value_type =
      std::conditional_t<isSet::value, Key, std::pair<const Key, T>>;

  /// @brief Type definition for representing size in BPlusTree
  using size_type = size_t;

  /// @brief Type definition for difference type in BPlusTree
  using difference_type = std::ptrdiff_t;

  /// @brief Type definition for key comparison function object in BPlusTree
  using key_compare = Compare;

  /// @brief Type definition for leaf node allocator in BPlusTree
  using allocator_type = Allocator;

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
                     const Indexer &indexor = Indexer())
      : m_comp(comp), m_allocator(alloc), m_indexor(indexor) {}

  /// @brief Allocator-based constructor
  /// @details It allows configuration of allocator and indexer.
  /// @param alloc Allocator configuration.
  /// @param indexor Indexer configuration. Defaults to Indexer().
  explicit BPlusTree(const Allocator &alloc, const Indexer &indexor = Indexer())
      : BPlusTree(Compare(), alloc, indexor) {}

  /// @brief Comparator & Indexer-based constructor
  /// @details It allows configuration of comparator and indexer.
  /// @param comp Comparator configuration.
  /// @param indexor Indexer configuration.
  explicit BPlusTree(const Compare &comp, const Indexer &indexor)
      : BPlusTree(comp, Allocator(), indexor) {}

  /// @brief Indexer-based constructor
  /// @details It allows configuration of only indexer.
  /// @param indexor Indexer configuration.
  explicit BPlusTree(const Indexer &indexor)
      : BPlusTree(Compare(), Allocator(), indexor) {}

  /// @brief Iterator-based constructor
  /// @details Allows input of range and configuration of comparator, allocator
  /// and indexer.
  /// @param [in] first Iterator pointing to the start of input range.
  /// @param [in] last Iterator pointing to the end of input range.
  /// @param comp Comparator.
  /// @param alloc Allocator. Defaults to Allocator().
  /// @param indexor Indexer. Defaults to Indexer().
  template <ValueInputIterator<value_type> InputIt>
  BPlusTree(InputIt first, InputIt last, const Compare &comp,
            const Allocator &alloc = Allocator(),
            const Indexer &indexor = Indexer());

  /// @brief Iterator and Allocator-based constructor
  /// @details Allows input of range and configuration of allocator and indexer.
  /// @param [in] first Iterator pointing to the start of input range.
  /// @param [in] last Iterator pointing to the end of input range.
  /// @param alloc Allocator.
  /// @param indexor Indexer. Defaults to Indexer().
  template <ValueInputIterator<value_type> InputIt>
  BPlusTree(InputIt first, InputIt last, const Allocator &alloc,
            const Indexer &indexor = Indexer())
      : BPlusTree(first, last, Compare(), alloc, indexor) {}

  /// @brief Iterator and Comparator & Indexer-based constructor
  /// @details Allows input of range and configuration of comparator and
  /// indexer.
  /// @param [in] first Iterator pointing to the start of input range.
  /// @param [in] last Iterator pointing to the end of input range.
  /// @param comp Comparator.
  /// @param indexor Indexer.
  template <ValueInputIterator<value_type> InputIt>
  BPlusTree(InputIt first, InputIt last, const Compare &comp,
            const Indexer &indexor)
      : BPlusTree(first, last, comp, Allocator(), indexor) {}

  /// @brief Iterator and Indexer-based constructor
  /// @details Allows input of range and configuration of indexer.
  /// @param [in] first Iterator pointing to the start of input range.
  /// @param [in] last Iterator pointing to the end of input range.
  /// @param indexor Indexer.
  template <ValueInputIterator<value_type> InputIt>
  BPlusTree(InputIt first, InputIt last, const Indexer &indexor)
      : BPlusTree(first, last, Compare(), Allocator(), indexor) {}

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
  /// @param alloc Allocator. Defaults to Allocator().
  /// @param indexor Indexer. Defaults to Indexer().
  BPlusTree(std::initializer_list<value_type> init, const Compare &comp,
            const Allocator &alloc = Allocator(),
            const Indexer &indexor = Indexer());

  /// @brief Iterator and Allocator-based constructor
  /// @details Allows input of range and configuration of allocator and indexer.
  /// @param [in] first Iterator pointing to the start of input range.
  /// @param [in] last Iterator pointing to the end of input range.
  /// @param alloc Allocator.
  /// @param indexor Indexer. Defaults to Indexer().
  BPlusTree(std::initializer_list<value_type> init, const Allocator &alloc,
            const Indexer &indexor = Indexer())
      : BPlusTree(init, Compare(), alloc, indexor) {}

  /// @brief Iterator and Comparator & Indexer-based constructor
  /// @details Allows input of range and configuration of comparator and
  /// indexer.
  /// @param [in] first Iterator pointing to the start of input range.
  /// @param [in] last Iterator pointing to the end of input range.
  /// @param comp Comparator.
  /// @param indexor Indexer.
  BPlusTree(std::initializer_list<value_type> init, const Compare &comp,
            const Indexer &indexor)
      : BPlusTree(init, comp, Allocator(), indexor) {}

  /// @brief Iterator and Indexer-based constructor
  /// @details Allows input of range and configuration of indexer.
  /// @param [in] first Iterator pointing to the start of input range.
  /// @param [in] last Iterator pointing to the end of input range.
  /// @param indexor Indexer.
  BPlusTree(std::initializer_list<value_type> init, const Indexer &indexor)
      : BPlusTree(init, Compare(), Allocator(), indexor) {}

  /// @}

  ~BPlusTree();

  /**
   * @name Element access
   * Accesor methods for the B+Tree based on the key
   * */
  /// @{
  mapped_type &at(const Key &key);
  const mapped_type &at(const Key &key) const;
  mapped_type &operator[](const Key &key);
  mapped_type &operator[](Key &&key);
  /// @}

  /**
   * @name Iterators
   * Iterator related methods
   * */
  /// @{
  iterator begin() noexcept;
  const_iterator begin() const noexcept;
  const_iterator cbegin() const noexcept;

  iterator end() noexcept;
  const_iterator end() const noexcept;
  const_iterator cend() const noexcept;

  reverse_iterator rbegin() noexcept;
  const_reverse_iterator rbegin() const noexcept;
  const_reverse_iterator crbegin() const noexcept;

  reverse_iterator rend() noexcept;
  const_reverse_iterator rend() const noexcept;
  const_reverse_iterator crend() const noexcept;
  /// @}
  /**
   * @name Capacity
   * Observer methods for the B+Tree capacity
   * */
  /// @{
  [[nodiscard]] bool empty() const noexcept;
  [[nodiscard]] size_type size() const noexcept;
  [[nodiscard]] size_type max_size() const noexcept;
  /// @}
  /**
   * @name Modifiers
   * Methods for modifying the B+Tree
   * */
  /// @{
  void clear() noexcept;

  // Insert

  std::pair<iterator, bool> insert(const value_type &value);

  template <rvalue_constructible_from<value_type> P>
  std::pair<iterator, bool> insert(P &&value);
  std::pair<iterator, bool> insert(value_type &&value);
  iterator insert(const_iterator position, const value_type &value);
  template <rvalue_constructible_from<value_type> P>
  iterator insert(const_iterator position, P &&value);
  iterator insert(const_iterator position, value_type &&value);

  template <ValueInputIterator<value_type> InputIt>
  void insert(InputIt first, InputIt last);

  void insert(std::initializer_list<value_type> ilist);

  // insert_or_assign
  template <MappedAssignable<mapped_type> P>
  std::pair<iterator, bool> insert_or_assign(const key_type &key, P &&obj);
  template <MappedAssignable<mapped_type> P>
  std::pair<iterator, bool> insert_or_assign(key_type &&key, P &&obj);
  template <MappedAssignable<mapped_type> P>
  iterator insert_or_assign(const_iterator hint, const key_type &key, P &&obj);
  template <MappedAssignable<mapped_type> P>
  iterator insert_or_assign(const_iterator hint, key_type &&key, P &&obj);

  // emplace
  template <std::constructible_from<value_type>... Args>
  std::pair<iterator, bool> emplace(Args &&...args);

  // emplace_hint
  template <std::constructible_from<value_type>... Args>
  iterator emplace_hint(const_iterator hint, Args &&...args);

  // try_emplace
  template <std::constructible_from<mapped_type>... Args>
  std::pair<iterator, bool> try_emplace(const key_type &key, Args &&...args);

  template <std::constructible_from<mapped_type>... Args>
  std::pair<iterator, bool> try_emplace(key_type &&key, Args &&...args);

  template <std::constructible_from<mapped_type>... Args>
  iterator try_emplace(const_iterator hint, const key_type &key,
                       Args &&...args);
  template <std::constructible_from<mapped_type>... Args>
  iterator try_emplace(const_iterator hint, key_type &&key, Args &&...args);

  // erase
  iterator erase(iterator position);
  iterator erase(const_iterator position);
  iterator erase(const_iterator first, const_iterator last);
  size_type erase(const key_type &key);
  template <ComparableKey<key_type> K> size_type erase(const K &key_compare);

  // swap
  void swap(BPlusTree &other) noexcept(
      std::allocator_traits<Allocator>::propagate_on_container_swap::value ||
      std::allocator_traits<Allocator>::is_always_equal::value);
  /// @}

  /**
   * @name Lookup
   * Methods for looking up elements in the B+Tree
   * */
  /// @{
  [[nodiscard]] size_type count(const Key &key) const;
  template <ComparableKey<key_type> K>
  [[nodiscard]] size_type count(const K &key_compare) const;

  [[nodiscard]] iterator find(const Key &key);
  [[nodiscard]] const_iterator find(const Key &key) const;

  template <ComparableKey<key_type> K>
  [[nodiscard]] iterator find(const K &key);
  template <ComparableKey<key_type> K>
  [[nodiscard]] const_iterator find(const K &key) const;

  bool contains(const Key &key) const;
  template <ComparableKey<key_type> K> bool contains(const K &key) const;

  std::pair<iterator, iterator> equal_range(const Key &key);
  std::pair<const_iterator, const_iterator> equal_range(const Key &key) const;

  template <ComparableKey<key_type> K>
  std::pair<iterator, iterator> equal_range(const K &key);
  template <ComparableKey<key_type> K>
  std::pair<const_iterator, const_iterator> equal_range(const K &key) const;

  iterator lower_bound(const Key &key);
  const_iterator lower_bound(const Key &key) const;

  template <ComparableKey<key_type> K> iterator lower_bound(const K &key);
  template <ComparableKey<key_type> K> const_iterator lower_bound(const K &key);

  iterator upper_bound(const Key &key);
  const_iterator upper_bound(const Key &key) const;
  template <ComparableKey<key_type> K> iterator upper_bound(const K &key);
  template <ComparableKey<key_type> K> const_iterator upper_bound(const K &key);

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
  indexor m_indexor;
};

template <size_t M, properKeyValue Key, std::predicate<Key, Key> Compare,
          typename Indexer>
  requires(M >= MIN_DEGREE)
class BPlusTree<M, Key, Compare, Indexer>
    : public BPlusTree<M, Key, Key, Compare, Indexer> {};

template <size_t M, properKeyValue Key, std::predicate<Key, Key> Compare>
  requires(M >= MIN_DEGREE)
class BPlusTree<M, Key, Compare> : public BPlusTree<M, Key, Key, Compare> {};

#define BPLUS_TEMPLATES                                                        \
  size_t M, properKeyValue Key, properKeyValue T,                              \
      std::predicate<Key, Key> Compare, typename Indexer, class Allocator,     \
      size_t CHILD_COUNT, size_t KEY_COUNT

#define BPLUS_TEMPLATE_PARAMS                                                  \
  M, Key, T, Compare, Indexer, Allocator, CHILD_COUNT, KEY_COUNT

template <BPLUS_TEMPLATES>
using isSet =
    std::integral_constant<bool, std::is_same<Indexer, Identity>::value &&
                                     std::is_same<Key, T>::value>;

template <BPLUS_TEMPLATES>
using value_type = std::conditional_t<isSet<BPLUS_TEMPLATE_PARAMS>::value, Key,
                                      std::pair<const Key, T>>;

/******************
*******************
* Implementations *
*******************
******************/

// *** Constructors *** //
template <BPLUS_TEMPLATES> BPlusTree<BPLUS_TEMPLATE_PARAMS>::~BPlusTree() {
  delete m_root;
}

template <BPLUS_TEMPLATES>
template <ValueInputIterator<value_type<BPLUS_TEMPLATE_PARAMS>> InputIt>
BPlusTree<BPLUS_TEMPLATE_PARAMS>::BPlusTree(InputIt first, InputIt last,
                                            const Compare &comp,
                                            const Allocator &alloc,
                                            const Indexer &indexor)
    : m_comp(comp), m_allocator(alloc), m_indexor(indexor) {

  insert(first, last);
}

template <BPLUS_TEMPLATES>
BPlusTree<BPLUS_TEMPLATE_PARAMS>::BPlusTree(const BPlusTree &other)
    : m_comp(other.m_comp),
      m_allocator(std::allocator_traits<allocator_type>::
                      select_on_container_copy_construction(other.m_allocator)),
      m_indexor(other.m_indexor) {

  if (other.m_root != nullptr) {
    // InternalNode or LeafNode
    m_root = new std::remove_pointer_t<decltype(other.m_root)>(*other.m_root);
  } else {
    m_root = nullptr;
  }
}

template <BPLUS_TEMPLATES>
BPlusTree<BPLUS_TEMPLATE_PARAMS>::BPlusTree(const BPlusTree &other,
                                            const Allocator &alloc)
    : m_comp(other.m_comp), m_allocator(alloc), m_indexor(other.m_indexor) {

  if (alloc == other.m_allocator) {
    if (other.m_root != nullptr) {
      // InternalNode or LeafNode
      m_root = new std::remove_pointer_t<decltype(other.m_root)>(*other.m_root);
    } else {
      m_root = nullptr;
    }
    return;
  }

  throw std::runtime_error("Copy constructor with allocator not implemented if "
                           "allocators are not equal");
}

template <BPLUS_TEMPLATES>
BPlusTree<BPLUS_TEMPLATE_PARAMS>::BPlusTree(BPlusTree &&other) noexcept
    : m_root(std::exchange(other.m_root, nullptr)),
      m_allocator(std::move(other.m_allocator)),
      m_comp(std::move(other.m_allocator)),
      m_indexor(std::move(other.m_indexor)) {}

template <BPLUS_TEMPLATES>
BPlusTree<BPLUS_TEMPLATE_PARAMS>::BPlusTree(BPlusTree &&other,
                                            const Allocator &alloc)
    : m_comp(other.m_comp), m_indexor(other.m_indexor) {

  if (alloc == other.m_allocator) {
    m_allocator = std::move(other.m_allocator);
    m_root = std::exchange(other.m_root, nullptr);
    return;
  }

  throw std::runtime_error("Move constructor with allocator not implemented if "
                           "allocator is not equal to other allocator");
}

template <BPLUS_TEMPLATES>
BPlusTree<BPLUS_TEMPLATE_PARAMS> &
BPlusTree<BPLUS_TEMPLATE_PARAMS>::operator=(BPlusTree &&other) noexcept {

  if (this != &other) {

    this->clear();

    m_root = std::exchange(other.m_root, nullptr);
    m_allocator = std::move(other.m_allocator);
    m_comp = std::move(other.m_comp);
    m_indexor = std::move(other.m_indexor);
  }

  return *this;
}

template <BPLUS_TEMPLATES>
BPlusTree<BPLUS_TEMPLATE_PARAMS> &
BPlusTree<BPLUS_TEMPLATE_PARAMS>::operator=(const BPlusTree &other) {

  if (this != &other) {
    this->clear();
    m_allocator = other.m_allocator;
    m_comp = other.m_comp;
    m_indexor = other.m_indexor;

    if (other.m_root != nullptr) {
      // InternalNode or LeafNode
      m_root = new std::remove_pointer_t<decltype(other.m_root)>(*other.m_root);
    } else {
      m_root = nullptr;
    }
  }
  return *this;
}

template <BPLUS_TEMPLATES>
BPlusTree<BPLUS_TEMPLATE_PARAMS> &BPlusTree<BPLUS_TEMPLATE_PARAMS>::operator=(
    std::initializer_list<value_type> ilist) {

  this->clear();

  insert(ilist);

  return *this;
}

template <BPLUS_TEMPLATES>
BPlusTree<BPLUS_TEMPLATE_PARAMS>::BPlusTree(
    std::initializer_list<value_type> init, const Compare &comp,
    const Allocator &alloc, const Indexer &indexor)
    : m_comp(comp), m_allocator(alloc), m_indexor(indexor) {
  insert(init);
}

// *** Insertion *** //

template <BPLUS_TEMPLATES>
auto BPlusTree<BPLUS_TEMPLATE_PARAMS>::insert(const value_type &value)
    -> std::pair<iterator, bool> {}

template <BPLUS_TEMPLATES>
template <rvalue_constructible_from<value_type<BPLUS_TEMPLATE_PARAMS>> P>
auto BPlusTree<BPLUS_TEMPLATE_PARAMS>::insert(P &&value)
    -> std::pair<iterator, bool> {}
template <BPLUS_TEMPLATES>
auto BPlusTree<BPLUS_TEMPLATE_PARAMS>::insert(value_type &&value)
    -> std::pair<iterator, bool> {}
template <BPLUS_TEMPLATES>
auto BPlusTree<BPLUS_TEMPLATE_PARAMS>::insert(const_iterator position,
                                              const value_type &value)
    -> iterator {}
template <BPLUS_TEMPLATES>
template <rvalue_constructible_from<value_type<BPLUS_TEMPLATE_PARAMS>> P>
auto BPlusTree<BPLUS_TEMPLATE_PARAMS>::insert(const_iterator position,
                                              P &&value) -> iterator {}
template <BPLUS_TEMPLATES>
auto BPlusTree<BPLUS_TEMPLATE_PARAMS>::insert(const_iterator position,
                                              value_type &&value) -> iterator {}

template <BPLUS_TEMPLATES>
template <ValueInputIterator<value_type<BPLUS_TEMPLATE_PARAMS>> InputIt>
void BPlusTree<BPLUS_TEMPLATE_PARAMS>::insert(InputIt first, InputIt last) {}

template <BPLUS_TEMPLATES>
void BPlusTree<BPLUS_TEMPLATE_PARAMS>::insert(
    std::initializer_list<value_type> ilist) {}

#endif // !BPlusTree_HPP
