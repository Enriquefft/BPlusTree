#include "Public.hpp"

int main() {
  // Private c; // This should throw error
  Public p;
  p.foo();
  return 0;
}
