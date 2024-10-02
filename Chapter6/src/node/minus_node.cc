#include "../../Include/node/minus_node.h"
#include "../../Include/type/integer_type.h"

MinusNode::MinusNode(Node *in) : Node({nullptr, in}) {}

std::string MinusNode::label() { return "Minus"; }
std::string MinusNode::glabel() { return "-"; }

std::ostringstream &MinusNode::print_1(std::ostringstream &builder) {
  builder << "(-";
  in(1)->print_0(builder);
  builder << ")";
  return builder;
}

Type *MinusNode::compute() {
  auto i0 = dynamic_cast<TypeInteger *>(in(1)->type_);
  if (i0) {
    return i0->isConstant() ? TypeInteger::constant(-i0->value()) : i0;
  }
  return &Type::BOTTOM;
}

Node* MinusNode::idealize() {
  return nullptr;
}