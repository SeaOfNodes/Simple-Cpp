#include "../../Include/node/sub_node.h"
#include "../../Include/type/integer_type.h"

SubNode::SubNode(Node *lhs, Node *rhs) : Node({nullptr, lhs, rhs}) {}
std::string SubNode::label() { return "Sub"; }
std::string SubNode::glabel() { return "-"; }
std::ostringstream &SubNode::print_1(std::ostringstream &builder,
                                     Tomi::Vector<bool> &visited) {
  builder << "(";
  in(1)->print_0(builder, visited);

  builder << "-";
  in(2)->print_0(builder, visited);

  builder << ")";

  return builder;
}

Type *SubNode::compute() {
  if (in(1) == in(2))
    return TypeInteger::ZERO;
  auto i0 = dynamic_cast<TypeInteger *>(in(1)->type_);
  auto i1 = dynamic_cast<TypeInteger *>(in(2)->type_);
  if (i0->isConstant() && i1->isConstant()) {
    return TypeInteger::constant(i0->value() - i1->value());
  }
  return in(1)->type_->meet(in(2)->type_);
}

Node *SubNode::idealize() { return nullptr; }
Node *SubNode::copy(Node *lhs, Node *rhs) { return alloc.new_object<SubNode>(lhs, rhs); }