#include <iostream>
#include <map>

int main() {

  std::map<char, int> first = {{'a', 1}, {'b', 2}, {'c', 3}};

  std::map<char, int> second = {{'x', 3}};

  using allocator_type = std::map<char, int>::allocator_type;

  if (std::allocator_traits<
          allocator_type>::propagate_on_container_copy_assignment::value) {
    std::cout << "The allocators are propagated\n";
  }

  first.clear();

  std::copy(first.begin(), first.end(), second.begin());

  return 0;
}
