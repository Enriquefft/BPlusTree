#ifndef CONCEPTS_B_PLUS_TREE_HPP
#define CONCEPTS_B_PLUS_TREE_HPP

#include "BPlusTemplate.hpp"
#include <iterator>
#include <utility>

/// @defgroup Concepts B+Tree concepts
/// @name Concepts
/// @brief Concepts that the B+Tree members use
/// @{

/**
 * @brief Concept for a proper key value
 * @details
 * A proper key value is a value that can be used as a key in the tree, it must
 * be copy constructible and not a function
 * */
template <typename Key>
concept properKeyValue = std::copy_constructible<Key>;

template <typename T>
concept IsAllocator =
    requires(T alloc, std::size_t n) {
      typename T::value_type;
      { alloc.allocate(n) } -> std::same_as<typename T::value_type *>;
      { alloc.deallocate(std::declval<typename T::value_type *>(), n) };
    };

template <typename C, typename Key>
concept ComparableKey = std::equality_comparable_with<Key, C>;

template <typename F, typename Key, typename T>
concept Indexor = std::regular_invocable<F, T> &&
                  std::convertible_to<std::invoke_result_t<F, T>, Key>;

template <typename P, typename value_type>
concept rvalue_constructible_from =
    std::destructible<value_type> && std::is_constructible_v<value_type, P &&>;

template <typename P, typename mapped_type>
concept MappedAssignable = std::assignable_from<mapped_type &, P &&>;

template <typename InputIt, typename value_type>
concept ValueInputIterator =
    std::input_iterator<InputIt> &&
    std::constructible_from<value_type,
                            typename std::iterator_traits<InputIt>::value_type>;

template <typename T, typename... Args>
concept InverseConstructibleFrom = (std::constructible_from<Args, T> && ...);

struct InsertResult {
  bool inserted;
  bool alreadyExists;
  bool wasFull;

  // spaceship operator
  [[nodiscard]] friend constexpr auto
  operator==(const InsertResult &lhs, const InsertResult &rhs) noexcept {
    return lhs.inserted == rhs.inserted &&
           lhs.alreadyExists == rhs.alreadyExists && lhs.wasFull == rhs.wasFull;
  }
  [[nodiscard]] friend constexpr auto
  operator!=(const InsertResult &lhs, const InsertResult &rhs) noexcept {
    return !(lhs == rhs);
  }

  // bool
  [[nodiscard]] constexpr operator bool() const noexcept { return inserted; }
  [[nodiscard]] constexpr bool operator!() const noexcept { return !inserted; }
};

namespace INSERTION {
[[maybe_unused]] constexpr InsertResult SUCCESS{true, false, false};
[[maybe_unused]] constexpr InsertResult ALREADY_EXISTS{false, true, false};
[[maybe_unused]] constexpr InsertResult WAS_FULL{false, false, true};
} // namespace INSERTION

/// @}

// Declare B+Tree class
template <BPLUS_TEMPLATES> class BPlusTree;

#endif // !CONCEPTS_B_PLUS_TREE_HPP
