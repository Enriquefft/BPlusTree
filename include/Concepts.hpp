#ifndef CONCEPTS_B_PLUS_TREE_HPP
#define CONCEPTS_B_PLUS_TREE_HPP

#include <iterator>
#include <utility>

/// @defgroup Concepts B+Tree concepts
/// @name Concepts
/// @brief Concepts that the B+Tree members use
/// @{

template <typename C, typename Key>
concept ComparableKey = std::equality_comparable_with<Key, C>;

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

/// @}

/**
 * @brief Default Indexor function: Identity
 * @details
 * This function is used to extract the key from the value when the key is the
 * value, in other words, it returns itself
 * */
struct Identity {
  /**
   * @brief Indexor function
   * @param v Value to return
   */
  template <typename U>
  auto operator()(U &&v) const -> decltype(std::forward<U>(v)) {
    return std::forward<U>(v);
  }
  /**
   * @brief Check if the function is the identity
   * @return true
   */
  [[nodiscard]] static bool isIdentity() { return true; }
};

// Trivial, unusable indexor, not meant to work
template <typename Key> struct UnusableIndexor {
  template <typename T> auto operator()(T && /*unused*/) const -> Key {
    return {};
  }
  [[nodiscard]] static bool isIdentity() { return false; }
};

// specifies that F is a callable that accepts T as it argument, it can be
// invoked with it to return Key
template <typename F, typename Key, typename T>
concept Indexor = std::regular_invocable<F, T> &&
                  std::convertible_to<std::invoke_result_t<F, T>, Key>;

/**
 * @brief Concept for a proper key value
 * @details
 * A proper key value is a value that can be used as a key in the tree, it must
 * be copy constructible and not a function
 * */
template <typename Key>
concept properKeyValue = std::copy_constructible<Key> && !
std::is_function_v<std::remove_pointer_t<Key>>;

#endif // !CONCEPTS_B_PLUS_TREE_HPP
