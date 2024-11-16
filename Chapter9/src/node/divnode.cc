#include "../../Include/node/divnode.h"
#include "../../Include/type/integer_type.h"

DivNode::DivNode(Node *lhs, Node *rhs) : Node({nullptr, lhs, rhs}) {}

std::string DivNode::label() { return "Div"; }
std::string DivNode::glabel() { return "//"; }

std::ostringstream &DivNode::print_1(std::ostringstream &builder,
                                     Tomi::Vector<bool> &visited) {
  builder << "(";
  in(1)->print_0(builder, visited);
  builder << "/";
  in(2)->print_0(builder, visited);
  return builder;
}

Type *DivNode::compute() {
  auto i0 = dynamic_cast<TypeInteger *>(in(1)->type_);
  auto i1 = dynamic_cast<TypeInteger *>(in(2)->type_);
  if (i0 && i1) {
    if (i0->isConstant() && i1->isConstant()) {
      return i1->value() == 0
                 ? TypeInteger::ZERO
                 : TypeInteger::constant(i0->value() / i1->value());
    }
  }
  return in(1)->type_->meet(in(2)->type_);
}

Node *DivNode::idealize() { return nullptr; }

Node *DivNode::copy(Node *lhs, Node *rhs) { return new DivNode(lhs, rhs); }
