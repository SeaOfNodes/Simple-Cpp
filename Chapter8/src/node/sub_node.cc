#include "../../Include/node/sub_node.h"

SubNode::SubNode(Node *lhs, Node *rhs) : Node({nullptr, lhs, rhs}) {}
std::string SubNode::label() { return "Sub"; }
std::string SubNode::glabel() { return "-"; }
std::ostringstream &SubNode::print_1(std::ostringstream &builder, std::vector<bool>& visited) {
  builder << "(";
  in(1)->print_0(builder, visited);

  builder << "-";
  in(2)->print_0(builder, visited);

  builder << ")";

  return builder;
}

Type *SubNode::compute() { return &Type::BOTTOM; }

Node *SubNode::idealize() { return nullptr; }
Node *SubNode::copy(Node *lhs, Node *rhs) { return new SubNode(lhs, rhs); }