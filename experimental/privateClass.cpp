
#include <iostream>
class Private {
protected:
  Private() = default;
  Private(int x) { std::cout << "Private(int x): " << x << "\n"; }
  Private(double y) { std::cout << "Private(double y): " << y << "\n"; }

public:
  void foo() { std::cout << "FOO\n"; }
};

class Public : public Private {
public:
  using Private::Private; // This brings all constructors from Private into
                          // Public's scope
};

int main() {
  Public p1{};     // Uses default constructor
  Public p2{42};   // Uses Private(int x)
  Public p3{3.14}; // Uses Private(double y)

  p1.foo(); // foo should be accessible
  return 0;
}
