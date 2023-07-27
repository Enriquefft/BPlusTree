#include <chrono>
#ifndef BPlusTree_HPP

#include <functional>

constexpr size_t MIN_DEGREE = 3;

/**
 * @brief Default Indexor function
 * @details
 * This function is used to extract the key from the value when the key is the
 * value, in other words, it returns itself
 * */
struct Identity {
  template <typename U>
  auto operator()(U &&v) const -> decltype(std::forward<U>(v)) {
    return std::forward<U>(v);
  }
  [[nodiscard]] static bool isIdentity() { return true; }
};

/**
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
template <size_t M, typename Key, std::copy_constructible T = Key,
          std::predicate<Key, Key> Compare = std::less<Key>,
          typename Indexer = Identity,
          class Allocator = std::allocator<const Key>, //
          size_t CHILD_COUNT = M, size_t KEY_COUNT = M - 1>
  requires(M >= MIN_DEGREE)
class BPlusTree {
  // potential missuse of CHILD_COUNT and KEY_COUNT
  static_assert(CHILD_COUNT == M,
                "CHILD_COUNT must not be manually changed\nIf you want to "
                "change the order of the "
                "tree, change M\nCHILD_COUNT is used for internal purposes");
  static_assert(KEY_COUNT == M - 1,
                "KEY_COUNT must not be manually changed\nIf you want to change "
                "the order of "
                "the tree, change M\nKEY_COUNT is used for internal purposes");
  template <bool isConst> class BPlusTreeIterator;

public:
  // Typedefs
  using key_type = Key;
  using mapped_type = T;
  using value_type = Key;
  using size_type = size_t;
  using difference_type = std::ptrdiff_t;
  using key_compare = Compare;
  using allocator_type = Allocator;

  using reference = value_type &;
  using const_reference = const value_type &;
  using pointer = typename std::allocator_traits<Allocator>::pointer;
  using const_pointer =
      typename std::allocator_traits<Allocator>::const_pointer;
  using iterator = BPlusTreeIterator<false>;
  using const_iterator = BPlusTreeIterator<true>;
  using reverse_iterator = std::reverse_iterator<iterator>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;

  // Testing purposes
  using indexor = Indexer;

  BPlusTree() = default;

private:
  // Internal use classes

  class Node {
    std::array<Key, KEY_COUNT> keys;
    bool isLeaf; // Used to mantain:
                 // https://en.wikipedia.org/wiki/Liskov_substitution_principle
  };
  class LeafNode : public Node {
    std::array<T, KEY_COUNT> values;
    LeafNode *next;
    LeafNode *prev;
  };
  class InternalNode : public Node {
    std::array<Node *, CHILD_COUNT> children;
  };
  template <bool isConst> class BPlusTreeIterator {};

  // Private members
  Node *root{};
};

template <size_t M, typename Key, std::predicate<Key, Key> Compare>
  requires(M >= MIN_DEGREE)
class BPlusTree<M, Key, Compare> : public BPlusTree<M, Key, Key, Compare> {};

#endif // !BPlusTree_HPP
