#include "../../Include/node/return_node.h"
#include <initializer_list>
#include <type/tuple_type.h>

ReturnNode::ReturnNode(Node *ctrl, Node *data) : Node({ctrl, data}) {}

[[nodiscard]] Node *ReturnNode::ctrl() { return in(0); }
[[nodiscard]] Node *ReturnNode::expr() { return in(1); }

bool ReturnNode::isCFG() { return true; }

std::ostringstream &ReturnNode::print_1(std::ostringstream &builder, std::vector<bool>& visited) {
  builder << "return ";
  Node *expr1 = expr();
  expr1->print_0(builder, visited);
  builder << ";";
  return builder;
}

Type *ReturnNode::compute() {
  return alloc.new_object<TypeTuple>(std::initializer_list<Type*>{ctrl()->type_, expr()->type_});
}

std::string ReturnNode::label() { return "Return"; }

// for some reasons instead of getting ~ctrl im getting #
Node *ReturnNode::idealize() {
  if (ctrl()->type_ == &Type::XCONTROL) {
    return ctrl();
  }

  return nullptr;
}
