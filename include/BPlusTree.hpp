#ifndef BPlusTree_HPP
#define BPlusTree_HPP

#include "Concepts.hpp"
#include "Iterator.hpp"
#include "NodeHandler.hpp"

#include <cmath>
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

          IsAllocator Allocator = std::allocator<std::pair<const Key, T>>,
          bool isSet = false, size_t MAX_CHILDS = M, size_t MAX_KEYS = M - 1>
// Ugly solution, but allows for compile time usage of constants with 0
// overhead.

class BPlusTree {

  size_t C_MIN_CHILDS = static_cast<size_t>(std::ceil(M / 2));
  size_t C_MIN_KEYS = static_cast<size_t>(std::ceil(M / 2)) - 1;

  // M must be at least 3
  static_assert(M >= MIN_DEGREE, "M(B+Tree degree) must be at least 3");

  using NodeHandler_ =
      NodeHandler<M, Key, T, Compare, Allocator, isSet, MAX_CHILDS, MAX_KEYS>;

  using LeafNode = typename NodeHandler_::LeafNode_;
  using InternalNode = typename NodeHandler_::InternalNode_;

  template <bool isConst>
  using BPlusTreeIterator =
      BPlusTreeIterator<isConst, M, Key, T, Compare, Allocator, isSet,
                        MAX_CHILDS, MAX_KEYS>;

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
  using value_type = std::conditional_t<isSet, Key, std::pair<Key, T>>;

  /// @brief Type definition for representing size in BPlusTree
  using size_type = size_t;

  /// @brief Type definition for difference type in BPlusTree
  using difference_type = std::ptrdiff_t;

  /// @brief Type definition for key comparison function object in BPlusTree
  using key_compare = Compare;

  /// @brief Type definition for leaf node allocator in BPlusTree
  using allocator_type = Allocator;
  using allocator_traits = std::allocator_traits<Allocator>;

  /// @brief Type definition for reference to the value_type
  using reference = value_type &;

  /// @brief Type definition for constant reference to the value_type
  using const_reference = const value_type &;

  /// @brief Type definition for pointer to the element in BPlusTree
  using pointer = typename allocator_traits::pointer;

  /// @brief Type definition for constant pointer to the element in BPlusTree
  using const_pointer = typename allocator_traits::const_pointer;

  /// @brief Type definition for BPlusTree iterator
  using iterator = BPlusTreeIterator<false>;

  /// @brief Type definition for constant BPlusTree iterator
  using const_iterator = BPlusTreeIterator<true>;

  /// @brief Type definition for reverse BPlusTree iterator
  using reverse_iterator = std::reverse_iterator<iterator>;

  /// @brief Type definition for constant reverse BPlusTree iterator
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;

  /// @}

  /// @defgroup Constructors B+Tree Constructors
  /// @name Constructors
  /// @brief B+Tree constructors
  /// @details These constructors allow for any combination of comp, alloc and

protected:
  /// @{
  /// @brief Default constructor
  /// @details It uses default comparator, allocator.
  BPlusTree() : BPlusTree(Compare()) {}

  /// @brief Comprehensive constructor
  /// @details It allows configuration of comparator, allocator.
  /// @param comp Comparator configuration.
  /// @param alloc Allocator configuration. Defaults to Allocator().
  explicit BPlusTree(const Compare &comp, const Allocator &alloc = Allocator())
      : m_comp(comp), m_leaf_allocator(alloc) {}

  /// @brief Allocator-based constructor
  /// @details It allows for configuration of allocator.
  /// @param alloc Allocator configuration.
  explicit BPlusTree(const Allocator &alloc) : BPlusTree(Compare(), alloc) {}

  /// @brief Iterator-based constructor
  /// @details Allows input of range and configuration of comparator and
  /// allocator
  /// @param [in] first Iterator pointing to the start of input range.
  /// @param [in] last Iterator pointing to the end of input range.
  /// @param comp Comparator.
  /// @param alloc Allocator. Defaults to Allocator().
  template <ValueInputIterator<value_type> InputIt>
  BPlusTree(InputIt first, InputIt last, const Compare &comp,
            const Allocator &alloc = Allocator())
      : m_comp(comp), m_leaf_allocator(alloc) {
    insert(first, last);
  }

  /// @brief Iterator and Allocator-based constructor
  /// @details Allows input of range and configuration of allocator.
  /// @param [in] first Iterator pointing to the start of input range.
  /// @param [in] last Iterator pointing to the end of input range.
  /// @param alloc Allocator.
  template <ValueInputIterator<value_type> InputIt>
  BPlusTree(InputIt first, InputIt last, const Allocator &alloc)

      : BPlusTree(first, last, Compare(), alloc) {}

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

  /// @brief Initializer list constructor with compare and alloc
  /// @details Constructs a new object from the given initializer list
  /// @param init Initializer list to assign from.
  /// @param comp Comparator.
  /// @param alloc Allocator. Defaults to Allocator().
  BPlusTree(std::initializer_list<value_type> init, const Compare &comp,
            const Allocator &alloc = Allocator());

  /// @brief Iterator and Allocator-based constructor
  /// @details Allows input of range and configuration of allocator.
  /// @param [in] init Initializer list
  /// @param alloc Allocator.
  BPlusTree(std::initializer_list<value_type> init, const Allocator &alloc)
      : BPlusTree(init, Compare(), alloc) {}

public:
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

  /// @}

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
  std::pair<iterator, bool> insert(P &&value) {
    emplace(std::forward<P>(value));
  }

  std::pair<iterator, bool> insert(value_type &&value);

  iterator insert(const_iterator position, const value_type &value);

  template <rvalue_constructible_from<value_type> P>
  iterator insert(const_iterator position, P &&value) {
    emplace_hint(position, std::forward<P>(value));
  }

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
  template <InverseConstructibleFrom<value_type>... Args>
  std::pair<iterator, bool> emplace(Args &&...args);

  // emplace_hint
  template <InverseConstructibleFrom<value_type>... Args>
  iterator emplace_hint(const_iterator hint, Args &&...args);

  // try_emplace
  template <InverseConstructibleFrom<mapped_type>... Args>
  std::pair<iterator, bool> try_emplace(const key_type &key, Args &&...args);

  template <InverseConstructibleFrom<mapped_type>... Args>
  std::pair<iterator, bool> try_emplace(key_type &&key, Args &&...args);

  template <InverseConstructibleFrom<mapped_type>... Args>
  iterator try_emplace(const_iterator hint, const key_type &key,
                       Args &&...args);
  template <InverseConstructibleFrom<mapped_type>... Args>
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

  bool is_map() { return !isSet; }

  /// @}

private:
  // Private members
  NodeHandler_ m_root = nullptr;
  key_compare m_comp;
  allocator_type m_leaf_allocator;
};

#define BPLUS_TEMPLATES                                                        \
  size_t M, properKeyValue Key, properKeyValue T,                              \
      std::predicate<Key, Key> Compare, IsAllocator Allocator, bool isSet,     \
      size_t CHILD_COUNT, size_t KEY_COUNT

#define BPLUS_TEMPLATE_PARAMS                                                  \
  M, Key, T, Compare, Allocator, isSet, CHILD_COUNT, KEY_COUNT

/******************
*******************
* Implementations *
*******************
******************/

// *** Constructors *** //

template <BPLUS_TEMPLATES>
BPlusTree<BPLUS_TEMPLATE_PARAMS>::BPlusTree(const BPlusTree &other)
    : m_comp(other.m_comp),
      m_leaf_allocator(
          std::allocator_traits<allocator_type>::
              select_on_container_copy_construction(other.m_leaf_allocator)) {

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
    : m_comp(other.m_comp), m_leaf_allocator(alloc) {

  if (alloc == other.m_leaf_allocator) {
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
      m_leaf_allocator(std::move(other.m_leaf_allocator)),
      m_comp(std::move(other.m_comp)) {}

template <BPLUS_TEMPLATES>
BPlusTree<BPLUS_TEMPLATE_PARAMS>::BPlusTree(BPlusTree &&other,
                                            const Allocator &alloc)
    : m_comp(other.m_comp) {

  if (alloc == other.m_leaf_allocator) {
    m_leaf_allocator = std::move(other.m_leaf_allocator);
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
    m_leaf_allocator = std::move(other.m_leaf_allocator);
    m_comp = std::move(other.m_comp);
  }

  return *this;
}

template <BPLUS_TEMPLATES>
BPlusTree<BPLUS_TEMPLATE_PARAMS> &
BPlusTree<BPLUS_TEMPLATE_PARAMS>::operator=(const BPlusTree &other) {

  if (this != &other) {
    this->clear();
    m_leaf_allocator = other.m_leaf_allocator;
    m_comp = other.m_comp;

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
    const Allocator &alloc)
    : m_comp(comp), m_leaf_allocator(alloc) {
  insert(init);
}

// *** Insertion *** //

template <BPLUS_TEMPLATES>
auto BPlusTree<BPLUS_TEMPLATE_PARAMS>::insert(const value_type &value)
    -> std::pair<iterator, bool> {

  if (m_root == nullptr) {
    m_root = new LeafNode(m_leaf_allocator, m_comp);
  }
  m_root.insert(value);
}

template <BPLUS_TEMPLATES>
auto BPlusTree<BPLUS_TEMPLATE_PARAMS>::insert(value_type &&value)
    -> std::pair<iterator, bool> {
  if (m_root == nullptr) {
    m_root = new LeafNode(m_leaf_allocator, m_comp);
  }
  return m_root.insert(std::move(value));
}

template <BPLUS_TEMPLATES>
auto BPlusTree<BPLUS_TEMPLATE_PARAMS>::insert(const_iterator position,
                                              const value_type &value)
    -> iterator {
  if (m_root == nullptr) {
    m_root = new LeafNode(m_leaf_allocator, m_comp);
  }
  return m_root.insert(position, value);
}

template <BPLUS_TEMPLATES>
auto BPlusTree<BPLUS_TEMPLATE_PARAMS>::insert(const_iterator position,
                                              value_type &&value) -> iterator {
  if (m_root == nullptr) {
    m_root = new LeafNode(m_leaf_allocator, m_comp);
  }
  m_root.insert(position, std::move(value));
}

template <BPLUS_TEMPLATES>
void BPlusTree<BPLUS_TEMPLATE_PARAMS>::insert(
    std::initializer_list<value_type> ilist) {
  if (m_root == nullptr) {
    m_root = new LeafNode(m_leaf_allocator, m_comp);
  }
  m_root.insert(ilist);
}

#endif // !BPlusTree_HPP
