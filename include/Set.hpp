#ifndef SET_HPP
#define SET_HPP

#include "BPlusTree.hpp"

template <size_t M, properKeyValue Key,
          std::predicate<Key, Key> Compare = std::less<Key>,
          IsAllocator Allocator = std::allocator<Key>>
struct Set : public BPlusTree<M, Key, Key, Compare, Identity, Allocator, true,
                              M, M - 1> {

  // operator= is not inherited by default
  using BPlusTree<M, Key, Key, Compare, Identity, Allocator, true, M,
                  M - 1>::operator=;

  // Forwarding all constructors

  Set() : Set(Compare()) {}

  explicit Set(const Compare &comp, const Allocator &alloc = Allocator())
      : BPlusTree<M, Key, Key, Compare, Identity, Allocator, true, M, M - 1>(
            comp, alloc) {}

  explicit Set(const Allocator &alloc)
      : BPlusTree<M, Key, Key, Compare, Identity, Allocator, true, M, M - 1>(
            alloc) {}

  template <ValueInputIterator<Key> InputIt>
  Set(InputIt first, InputIt last, const Compare &comp,
      const Allocator &alloc = Allocator())
      : BPlusTree<M, Key, Key, Compare, Identity, Allocator, true, M, M - 1>(
            first, last, comp, alloc) {}

  template <ValueInputIterator<Key> InputIt>
  Set(InputIt first, InputIt last, const Allocator &alloc = Allocator())
      : BPlusTree<M, Key, Key, Compare, Identity, Allocator, true, M, M - 1>(
            first, last, alloc) {}

  Set(const Set &other)
      : BPlusTree<M, Key, Key, Compare, Identity, Allocator, true, M, M - 1>(
            other) {}

  Set(const Set &other, const Allocator &alloc)
      : BPlusTree<M, Key, Key, Compare, Identity, Allocator, true, M, M - 1>(
            other, alloc) {}

  Set(Set &&other) noexcept
      : BPlusTree<M, Key, Key, Compare, Identity, Allocator, true, M, M - 1>(
            std::move(other)) {}

  Set(Set &&other, const Allocator &alloc)
      : BPlusTree<M, Key, Key, Compare, Identity, Allocator, true, M, M - 1>(
            std::move(other), alloc) {}

  Set(std::initializer_list<Key> init, const Compare &comp,
      const Allocator &alloc = Allocator())
      : BPlusTree<M, Key, Key, Compare, Identity, Allocator, true, M, M - 1>(
            init, comp, alloc) {}

  Set(std::initializer_list<Key> init, const Allocator &alloc)
      : BPlusTree<M, Key, Key, Compare, Identity, Allocator, true, M, M - 1>(
            init, alloc) {}
};

#endif // !SET_HPP
