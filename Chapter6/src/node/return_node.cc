#include "../../Include/node/return_node.h"

#include <type/tuple_type.h>

ReturnNode::ReturnNode(Node *ctrl, Node *data) : Node({ctrl, data}) {}

[[nodiscard]] Node *ReturnNode::ctrl() { return in(0); }
[[nodiscard]] Node *ReturnNode::expr() { return in(1); }

bool ReturnNode::isCFG() { return true; }

std::ostringstream &ReturnNode::print_1(std::ostringstream &builder) {
  builder << "return ";
  Node *expr1 = expr();
  expr1->print_1(builder);
  builder << ";";
  return builder;
}

Type *ReturnNode::compute() {
  return new TypeTuple({ctrl()->type_, expr()->type_});
}

std::string ReturnNode::label() { return "Return"; }

Node *ReturnNode::idealize() {
  if (ctrl()->type_ == &Type::XCONTROL)
    return ctrl();
  return nullptr;
}
