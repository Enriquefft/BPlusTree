#ifndef ITERATOR_HPP
#define ITERATOR_HPP

#include "Concepts.hpp"

/**
 * @class BPlusTreeIterator
 * @brief Iterator for B+ tree.
 * @details The BPlusTreeIterator class is a bidirectional iterator which
 * follows the standard
 * */
template <BPLUS_TEMPLATES, bool isConst> class BPlusTreeIterator {

public:
  BPlusTreeIterator() = default;
};

#endif // !ITERATOR_HPP
