#ifndef SET_HPP
#define SET_HPP

#include "BPlusTree.hpp"

template <typename Key, typename value_type> struct SetIndexor {
  const Key &operator()(const value_type &pair) { return pair.first; }
};

[[maybe_unused]] static void foo() {

  SetIndexor<int, std::pair<int, int>> indexor;

  int val = indexor(std::pair<int, int>(1, 2));

  std::cout << val << std::endl;
}

template <size_t M, properKeyValue Key,
          std::predicate<Key, Key> Compare = std::less<Key>,
          IsAllocator Allocator = std::allocator<Key>>

struct Set : public BPlusTree<M, Key, Key, SetIndexor<Key, std::pair<Key, Key>>,
                              Compare, Allocator> {

  using indexor = SetIndexor<Key, Key>;

  // operator= is not inherited by default
  using BPlusTree<M, Key, Key, indexor, Compare, Allocator>::operator=;

  // Forwarding all constructors

  Set() : Set(Compare()) {}

  explicit Set(const Compare &comp, const Allocator &alloc = Allocator())
      : BPlusTree<M, Key, Key, indexor, Compare, Allocator>(comp, alloc) {}

  explicit Set(const Allocator &alloc)
      : BPlusTree<M, Key, Key, indexor, Compare, Allocator>(alloc) {}

  template <ValueInputIterator<Key> InputIt>
  Set(InputIt first, InputIt last, const Compare &comp,
      const Allocator &alloc = Allocator())
      : BPlusTree<M, Key, Key, indexor, Compare, Allocator>(first, last, comp,
                                                            alloc) {}

  template <ValueInputIterator<Key> InputIt>
  Set(InputIt first, InputIt last, const Allocator &alloc = Allocator())
      : BPlusTree<M, Key, Key, indexor, Compare, Allocator>(first, last,
                                                            alloc) {}

  Set(const Set &other)
      : BPlusTree<M, Key, Key, indexor, Compare, Allocator>(other) {}

  Set(const Set &other, const Allocator &alloc)
      : BPlusTree<M, Key, Key, indexor, Compare, Allocator>(other, alloc) {}

  Set(Set &&other) noexcept
      : BPlusTree<M, Key, Key, indexor, Compare, Allocator>(std::move(other)) {}

  Set(Set &&other, const Allocator &alloc)
      : BPlusTree<M, Key, Key, indexor, Compare, Allocator>(std::move(other),
                                                            alloc) {}

  Set(std::initializer_list<Key> init, const Compare &comp,
      const Allocator &alloc = Allocator())
      : BPlusTree<M, Key, Key, indexor, Compare, Allocator>(init, comp, alloc) {
  }

  Set(std::initializer_list<Key> init, const Allocator &alloc)
      : BPlusTree<M, Key, Key, indexor, Compare, Allocator>(init, alloc) {}
};

#endif // !SET_HPP
