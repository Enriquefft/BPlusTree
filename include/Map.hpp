#ifndef MAP_HPP
#define MAP_HPP

#include "BPlusTree.hpp"

template <size_t M, properKeyValue Key, properKeyValue T,
          std::predicate<Key, Key> Compare = std::less<Key>,
          Indexor<Key, T> Indexer = UnusableIndexor<Key>,
          IsAllocator Allocator = std::allocator<std::pair<const Key, T>>>
struct Map : public BPlusTree<M, Key, T, Compare, Indexer, Allocator, false, M,
                              M - 1> {

  // operator= is not inherited by default
  using BPlusTree<M, Key, T, Compare, UnusableIndexor<Key>, Allocator, false, M,
                  M - 1>::operator=;
  using value_type = std::pair<const Key, T>;

  // Forwarding all constructors

  Map() : Map(Compare()) {}

  explicit Map(const Compare &comp, const Allocator &alloc = Allocator(),
               const Indexer &indexor = Indexer())
      : BPlusTree<M, Key, T, Compare, Indexer, Allocator, false, M, M - 1>(
            comp, alloc, indexor) {}

  explicit Map(const Allocator &alloc, const Indexer &indexor = Indexer())
      : BPlusTree<M, Key, T, Compare, Indexer, Allocator, false, M, M - 1>(
            alloc, indexor) {}

  explicit Map(const Compare &comp, const Indexer &indexor)
      : BPlusTree<M, Key, T, Compare, Indexer, Allocator, false, M, M - 1>(
            comp, indexor) {}

  explicit Map(const Indexer &indexor)
      : BPlusTree<M, Key, T, Compare, Indexer, Allocator, false, M, M - 1>(
            indexor) {}

  template <ValueInputIterator<value_type> InputIt>
  Map(InputIt first, InputIt last, const Compare &comp,
      const Allocator &alloc = Allocator(), const Indexer &indexor = Indexer())
      : BPlusTree<M, Key, T, Compare, Indexer, Allocator, false, M, M - 1>(
            first, last, comp, alloc, indexor) {}

  template <ValueInputIterator<value_type> InputIt>
  Map(InputIt first, InputIt last, const Allocator &alloc,
      const Indexer &indexor = Indexer())
      : BPlusTree<M, Key, T, Compare, Indexer, Allocator, false, M, M - 1>(
            first, last, alloc, indexor) {}

  template <ValueInputIterator<value_type> InputIt>
  Map(InputIt first, InputIt last, const Compare &comp, const Indexer &indexor)
      : BPlusTree<M, Key, T, Compare, Indexer, Allocator, false, M, M - 1>(
            first, last, comp, indexor) {}

  template <ValueInputIterator<value_type> InputIt>
  Map(InputIt first, InputIt last, const Indexer &indexor)
      : BPlusTree<M, Key, T, Compare, Indexer, Allocator, false, M, M - 1>(
            first, last, indexor) {}

  Map(const Map &other)
      : BPlusTree<M, Key, T, Compare, Indexer, Allocator, false, M, M - 1>(
            other) {}

  Map(const Map &other, const Allocator &alloc)
      : BPlusTree<M, Key, T, Compare, Indexer, Allocator, false, M, M - 1>(
            other, alloc) {}

  Map(Map &&other) noexcept
      : BPlusTree<M, Key, T, Compare, Indexer, Allocator, false, M, M - 1>(
            std::move(other)) {}

  Map(Map &&other, const Allocator &alloc)
      : BPlusTree<M, Key, T, Compare, Indexer, Allocator, false, M, M - 1>(
            std::move(other), alloc) {}

  Map(std::initializer_list<value_type> init, const Compare &comp,
      const Allocator &alloc = Allocator(), const Indexer &indexor = Indexer())
      : BPlusTree<M, Key, T, Compare, Indexer, Allocator, false, M, M - 1>(
            init, comp, alloc, indexor) {}

  Map(std::initializer_list<value_type> init, const Allocator &alloc,
      const Indexer &indexor = Indexer())
      : BPlusTree<M, Key, T, Compare, Indexer, Allocator, false, M, M - 1>(
            init, alloc, indexor) {}

  Map(std::initializer_list<value_type> init, const Compare &comp,
      const Indexer &indexor)
      : BPlusTree<M, Key, T, Compare, Indexer, Allocator, false, M, M - 1>(
            init, comp, indexor) {}

  Map(std::initializer_list<value_type> init, const Indexer &indexor)
      : BPlusTree<M, Key, T, Compare, Indexer, Allocator, false, M, M - 1>(
            init, indexor) {}
};

#endif // !MAP_HPP
