#include <concepts>

template <typename T> class foo {
  using dependant = T;
  template <std::convertible_to<dependant> U> explicit foo(U var);
};

template <typename T> using dependant_t = T;

template <typename T>
template <std::convertible_to<dependant_t<T>> U>
foo<T>::foo(U var) {}
