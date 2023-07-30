#include <iostream>
#include <utility>

// template <typename T, typename U = T> class MapSet {
//
//   using value_type = std::pair<T, U>;
//
// public:
//   void foo() {
//     if constexpr (std::is_same_v<T, value_type>) {
//       std::cout << "T is value_type" << std::endl;
//     } else {
//       std::cout << "T is not value_type" << std::endl;
//     }
//   }
// };
//
// // template <typename T>
// // class MapSet<T, int> : public MapSet<T, int> { // partial specialization
// template <typename T> class MapSet<T, int> : public MapSet<T, int> {
//
//   using value_type = T;
// };

template <typename T, typename U = int> class MapSet {
public:
  using value_type = std::pair<T, U>;

  void foo() {
    if constexpr (std::is_same_v<T, value_type>) {
      std::cout << "T is value_type" << std::endl;
    } else {
      std::cout << "T is not value_type" << std::endl;
    }
  }
};

template <typename T> class MapSet<T, T> : public MapSet<T, std::pair<T, T>> {
public:
  using value_type = T;
};

int main(int argc, char *argv[]) {

  MapSet<int> mapset1;
  MapSet<int, int> mapset2;

  mapset1.foo(); // Expected: T is value_type
  mapset2.foo(); // Expected: T is not value_type

  return 0;
}
