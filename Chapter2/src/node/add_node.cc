#include "../../Include/node/add_node.h"
#include "../../Include/type/integer_type.h"

AddNode::AddNode(Node *lhs, Node *rhs) : Node({nullptr, lhs, rhs}) {}
std::string AddNode::label() { return "Add"; }
std::string AddNode::glabel() { return "+"; }

std::ostringstream &AddNode::print_1(std::ostringstream &builder) {
  builder << "(";
  in(1)->print_0(builder);
  builder << "+";
  in(2)->print_0(builder);
  builder << ")";
  return builder;
}

Type *AddNode::compute() {
  auto i0 = dynamic_cast<TypeInteger *>(in(1)->type_);
  auto i1 = dynamic_cast<TypeInteger *>(in(2)->type_);

  if (i0 && i1) {
    if (i0->isConstant() && i1->isConstant()) {
      return TypeInteger::constant(i0->value() + i1->value());
    }
  }
  return &Type::BOTTOM;
}

Node *AddNode::idealize() { return nullptr; }