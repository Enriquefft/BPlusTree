#ifndef LEAF_NODE_HPP
#define LEAF_NODE_HPP

#include "Concepts.hpp"
#include "Iterator.hpp"
#include <memory>

template <size_t M, properKeyValue Key, properKeyValue T,
          std::predicate<Key, Key> Compare, IsAllocator Allocator, bool isSet,
          size_t MAX_CHILDS, size_t MAX_KEYS>
class BPlusTree;

template <size_t M, properKeyValue Key, properKeyValue T,
          std::predicate<Key, Key> Compare, IsAllocator Allocator, bool isSet,
          size_t MAX_CHILDS, size_t MAX_KEYS>
class NodeHandler;

/**
 * @class LeafNode
 * @brief Leaf node for B+ tree.
 * @details The LeafNode class inherits from @ref Node "Node" and adds an
 * array of values and pointers to the next and previous leaf nodes.
 * */
template <size_t M, properKeyValue Key, properKeyValue T,
          std::predicate<Key, Key> Compare, IsAllocator Allocator, bool isSet,
          size_t MAX_CHILDS = M, size_t MAX_KEYS = M - 1>
class LeafNode {

  friend class BPlusTree<M, Key, T, Compare, Allocator, isSet, MAX_CHILDS,
                         MAX_KEYS>;
  friend class NodeHandler<M, Key, T, Compare, Allocator, isSet, MAX_CHILDS,
                           MAX_KEYS>;

private:
  using value_type = std::conditional_t<isSet, Key, std::pair<const Key, T>>;
  using iterator = BPlusTreeIterator<false, M, Key, T, Compare, Allocator,
                                     isSet, MAX_CHILDS, MAX_KEYS>;
  using const_iterator = BPlusTreeIterator<true, M, Key, T, Compare, Allocator,
                                           isSet, MAX_CHILDS, MAX_KEYS>;

  explicit LeafNode(Allocator &allocator, Compare &comparator)
      : m_allocator(allocator), m_comparator(comparator) {
    std::fill(m_values.begin(), m_values.end(), nullptr);
  }

  // Insert

  std::pair<iterator, InsertResult> insert(const value_type &value);

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
  void insert(InputIt first, InputIt last) {}

  void insert(std::initializer_list<value_type> ilist);

  std::array<value_type *, MAX_KEYS>
      m_values;               ///< Array of (M-1) values_types (key-value pairs)
  LeafNode *m_next = nullptr; ///< Pointer to next leaf node
  LeafNode *m_prev = nullptr; ///< Pointer to previous leaf node
  Allocator &m_allocator;     ///< Allocator for the node
  Compare &m_comparator;
};

#define BPLUS_TEMPLATES                                                        \
  size_t M, properKeyValue Key, properKeyValue T,                              \
      std::predicate<Key, Key> Compare, IsAllocator Allocator, bool isSet,     \
      size_t CHILD_COUNT, size_t KEY_COUNT

#define BPLUS_TEMPLATE_PARAMS                                                  \
  M, Key, T, Compare, Allocator, isSet, CHILD_COUNT, KEY_COUNT

template <BPLUS_TEMPLATES>
auto LeafNode<BPLUS_TEMPLATE_PARAMS>::insert(const value_type &value)
    -> std::pair<iterator, InsertResult> {

  // Find the position of the value in the sorted array
  auto position = m_values.begin();

  for (; position != m_values.end(); ++position) {
    if (*position == nullptr || m_comparator(value.first, (*position)->first)) {
      break;
    }
  }

  // If the value is already in the array, return false
  if (position != m_values.end() && (*position)->first == (value.first)) {
    return {{}, INSERTION::ALREADY_EXISTS};
  }

  // If the array is full, return false
  if (m_values.back() != nullptr) {
    return {{}, INSERTION::WAS_FULL};
  }

  // Shift the values to the right to make room for the new value
  std::move_backward(position, m_values.end() - 1, m_values.end());

  // Insert the new value
  *position = m_allocator.allocate(1);
  std::allocator_traits<Allocator>::construct(m_allocator, *position, value);
  return {{}, INSERTION::SUCCESS};
}

#endif // !LEAF_NODE_HPP
