#ifndef ITERATOR_HPP
#define ITERATOR_HPP

#include "Concepts.hpp"

/**
 * @class BPlusTreeIterator
 * @brief Iterator for B+ tree.
 * @details The BPlusTreeIterator class is a bidirectional iterator which
 * follows the standard
 * */
template <bool isConst, size_t M, properKeyValue Key, properKeyValue T,
          std::predicate<Key, Key> Compare, IsAllocator Allocator, bool isSet,
          size_t MAX_CHILDS, size_t MAX_KEYS>
class BPlusTreeIterator {

public:
  BPlusTreeIterator() = default;
};

#endif // !ITERATOR_HPP
