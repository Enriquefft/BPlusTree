#ifndef MAP_HPP
#define MAP_HPP

#include "BPlusTree.hpp"

template <size_t M, properKeyValue Key, properKeyValue T,
          std::predicate<Key, Key> Compare = std::less<Key>,
          IsAllocator Allocator = std::allocator<std::pair<const Key, T>>>
struct Map : public BPlusTree<M, Key, T, Compare, Allocator, false, M, M - 1> {

  // operator= is not inherited by default
  using BPlusTree<M, Key, T, Compare, Allocator, false, M, M - 1>::operator=;
  using value_type = std::pair<const Key, T>;

  // Forwarding all constructors

  Map() : Map(Compare()) {}

  explicit Map(const Compare &comp, const Allocator &alloc = Allocator())
      : BPlusTree<M, Key, T, Compare, Allocator, false, M, M - 1>(comp, alloc) {
  }

  explicit Map(const Allocator &alloc)
      : BPlusTree<M, Key, T, Compare, Allocator, false, M, M - 1>(alloc) {}

  template <ValueInputIterator<value_type> InputIt>
  Map(InputIt first, InputIt last, const Compare &comp,
      const Allocator &alloc = Allocator())
      : BPlusTree<M, Key, T, Compare, Allocator, false, M, M - 1>(first, last,
                                                                  comp, alloc) {
  }

  template <ValueInputIterator<value_type> InputIt>
  Map(InputIt first, InputIt last, const Allocator &alloc)
      : BPlusTree<M, Key, T, Compare, Allocator, false, M, M - 1>(first, last,
                                                                  alloc) {}

  Map(const Map &other)
      : BPlusTree<M, Key, T, Compare, Allocator, false, M, M - 1>(other) {}

  Map(const Map &other, const Allocator &alloc)
      : BPlusTree<M, Key, T, Compare, Allocator, false, M, M - 1>(other,
                                                                  alloc) {}

  Map(Map &&other) noexcept
      : BPlusTree<M, Key, T, Compare, Allocator, false, M, M - 1>(
            std::move(other)) {}

  Map(Map &&other, const Allocator &alloc)
      : BPlusTree<M, Key, T, Compare, Allocator, false, M, M - 1>(
            std::move(other), alloc) {}

  Map(std::initializer_list<value_type> init, const Compare &comp,
      const Allocator &alloc = Allocator())
      : BPlusTree<M, Key, T, Compare, Allocator, false, M, M - 1>(init, comp,
                                                                  alloc) {}

  Map(std::initializer_list<value_type> init, const Allocator &alloc)
      : BPlusTree<M, Key, T, Compare, Allocator, false, M, M - 1>(init, alloc) {
  }
};

#endif // !MAP_HPP
