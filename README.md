# B+Tree Library

A robust, efficient, and generic B+ Tree library for C++20.

## Table of Contents

- [Features](#features)
- [Usage](#usage)
  - [As a Map](#as-a-map)
  - [As a Set](#as-a-set)
- [Filesystem Operations](#filesystem-operations)
- [Future Plans](#future-plans)

## Features

This implementation of B+ Tree behaves similar to structures in std, and it has
most of the methods of std::map and std::set. It can be used:

- As a map
- As a set
- With the ability to handle operations with std::filesystem (planned, not yet
  implemented)

The class declaration follows:

```cpp
template <
size_t M,
properKeyValue Key,
properKeyValue T = Key,
std::predicate<Key, Key> Compare = std::less<Key>,
typename Indexer = Identity,
class Allocator = std::allocator<std::pair<const Key, T>>>
class BPlusTree {...};
```

## Usage

### As a Map

To be used similarly as `std::map`:

```cpp
cpp BPlusTree<5, std::string, int, std::less<>> treeAsMap;
treeAsMap.insert({"a_key", 1});
```

### As a Set

To be used similarly as `std::set`:

```cpp
cpp BPlusTree<3, int> treeAsSet;
treeAsSet.insert(1);
```

(Note that the examples are quite simple, for more complex examples, refer to
the std::map and std::set documentation)

## Filesystem Operations

While the implementation of direct std::filesystem support hasn't been
implemented yet, here are some potential use cases:

1. **Storing the tree to a file**: The representation of the B+ Tree data
   structure could be serialized and stored in a file. When running a program,
   the tree can be reloaded from this file, providing persistence to the stored
   data.

2. **Reading from a file**: The tree could also be used to read a structured
   file and sort the data entries, allowing for quick lookups, queries, or
   sorting data.

3. **Logging operations**: Operations on the tree (insertions, deletions, etc.)
   could be automatically logged to a file for review or debugging purposes.

## Future Plans

- Implement direct support for std::filesystem operations.
