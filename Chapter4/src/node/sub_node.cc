#include "../../Include/node/sub_node.h"

SubNode::SubNode(Node *lhs, Node *rhs) : Node({nullptr, lhs, rhs}) {}
std::string SubNode::label() { return "Sub"; }
std::string SubNode::glabel() { return "-"; }
std::ostringstream &SubNode::print_1(std::ostringstream &builder) {
  builder << "(";
  in(1)->print_0(builder);

  builder << "-";
  in(2)->print_0(builder);

  builder << ")";

  return builder;
}

Type *SubNode::compute() {

  return &Type::BOTTOM;

}

Node *SubNode::idealize() { return nullptr; }