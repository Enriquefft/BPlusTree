#ifndef CONCEPTS_B_PLUS_TREE_HPP
#define CONCEPTS_B_PLUS_TREE_HPP

#include <utility>

/// @defgroup Concepts B+Tree concepts
/// @name Concepts
/// @brief Concepts that the B+Tree members use
/// @{

template <typename Key, typename C>
concept ComparableKey = std::equality_comparable_with<Key, C>;

template <typename mapped_type, typename C>
concept MappedConvertible = std::convertible_to<C, mapped_type>;

template <typename Key, typename T, typename... Args>
concept PairConstructible = std::constructible_from<std::pair<Key, T>, Args...>;

template <typename T, typename... Args>
concept MappedConstructible = std::constructible_from<T, Args...>;

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

/**
 * @brief Concept for a functor
 * */
template <typename T, typename = void> struct is_functor : std::false_type {};

/// @cond
template <typename T>
struct is_functor<T, std::void_t<decltype(&T::operator())>> : std::true_type {};
/// @endcond

/**
 * @brief Concept for a non functor
 * */
template <typename T>
concept NonFunctor = !
is_functor<T>::value;

/**
 * @brief Concept for an indexer callable
 */
template <typename Functor, typename T, typename Key>
concept is_indexer = requires(Functor f, T v) {
                       { f(v) } -> std::convertible_to<Key>;
                     };

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