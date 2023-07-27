#ifndef CONCEPTS_B_PLUS_TREE_HPP
#define CONCEPTS_B_PLUS_TREE_HPP

#include <utility>

/**
 * @brief Default Indexor function
 * @details
 * This function is used to extract the key from the value when the key is the
 * value, in other words, it returns itself
 * */
struct Identity {
  template <typename U>
  auto operator()(U &&v) const -> decltype(std::forward<U>(v)) {
    return std::forward<U>(v);
  }
  [[nodiscard]] static bool isIdentity() { return true; }
};

template <typename T, typename = void> struct is_functor : std::false_type {};

template <typename T>
struct is_functor<T, std::void_t<decltype(&T::operator())>> : std::true_type {};

/**
 * @brief Concept for a non functor
 * */
template <typename T>
concept NonFunctor = !
is_functor<T>::value;

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
