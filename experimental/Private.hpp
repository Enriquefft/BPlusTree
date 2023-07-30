#include <iostream>
class Private {
protected:
  Private() = default;
  void foo() { std::cout << "FOO\n"; }
};
