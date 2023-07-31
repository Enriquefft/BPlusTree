#include <memory>
#include <utility>
using std::allocator;
using std::allocator_traits;
using std::pair;
int main(int argc, char *argv[]) {

  pair<const int, int> p1(1, 2);

  std::allocator<pair<const int, int>> alloc;

  auto *p = alloc.allocate(1);

  using T = pair<const int, int>;

  allocator_traits<allocator<T>>::construct<T, const T &>(alloc, p, p1);

  return 0;
}
