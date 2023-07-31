#include <iostream>
#include <variant>

template <typename T>
concept Node = requires(T t) {
                 { t.foo() } -> std::same_as<void>;
               };

class InnerNode {
public:
  void foo() { std::cout << "InnerNode::foo()" << std::endl; }
  void inner() { std::cout << "InnerNode::inner()" << std::endl; }
};

class OuterNode {
public:
  void foo() { std::cout << "OuterNode::foo()" << std::endl; }
  void outer() { std::cout << "OuterNode::outer()" << std::endl; }
};

struct NodeVisitor {
public:
  template <Node T> NodeVisitor(T t) : node(t) {}

  void foo() {
    std::visit([](auto &&arg) { arg.foo(); }, node);
  }

  // Only OuterNode has outer()
  void outer() {
    if (auto *node_ptr = std::get_if<OuterNode>(&node)) {
      node_ptr->outer();
    } else {
      throw std::runtime_error("No outer() method");
    }
  }
  void inner() {
    if (auto *node_ptr = std::get_if<InnerNode>(&node)) {
      node_ptr->inner();
    } else {
      throw std::runtime_error("No inner() method");
    }
  }

private:
  std::variant<InnerNode, OuterNode> node;
};

int main(int argc, char *argv[]) {
  NodeVisitor visitor(OuterNode{});

  visitor.foo();   // Should call OuterNode::foo()
  visitor.outer(); // Should call OuterNode::outer()

  visitor = InnerNode{};
  visitor.foo();   // Should call InnerNode::foo()
  visitor.inner(); // Should call InnerNode::inner()

  return 0;
}
