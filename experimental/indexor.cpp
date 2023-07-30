#include <iostream>
#include <utility>

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

// specifies that F is a callable that accepts T as it argument, it can be
// invoked with it to return Key
template <typename F, typename Key, typename T>
concept Indexor = std::regular_invocable<F, T> &&
                  std::convertible_to<std::invoke_result_t<F, T>, Key>;

int main() {

  static_assert(Indexor<Identity, int, std::string>);

  // if constexpr (Indexor<Identity, int, int>) {
  //   std::cout << "Identity<int, int> is an Indexor" << std::endl;
  // } else {
  //   std::cout << "Identity<int, int> is not an Indexor" << std::endl;
  // }
  // if constexpr (Indexor<Identity, int, float>) {
  //   std::cout << "Identity<int, float> is an Indexor" << std::endl;
  // } else {
  //   std::cout << "Identity<int, float> is not an Indexor" << std::endl;
  // }
  // if constexpr (Indexor<Identity, float, float>) {
  //   std::cout << "Identity<float, float> is an Indexor" << std::endl;
  // } else {
  //   std::cout << "Identity<float, float> is not an Indexor" << std::endl;
  // }
  // if constexpr (Indexor<Identity, int, std::string>) {
  //   std::cout << "Identity<int, string> is an Indexor" << std::endl;
  // } else {
  //   std::cout << "Identity<int, string> is not an Indexor" << std::endl;
  // }

  return 0;
}
