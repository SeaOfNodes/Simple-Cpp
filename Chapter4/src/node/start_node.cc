#include "../../Include/node/start_node.h"
#include "../../Include/type/integer_type.h"

StartNode::StartNode() {}
bool StartNode::isCFG() const { return true; }
std::ostringstream &StartNode::print_1(std::ostringstream &builder) {
  builder << label();
  return builder;
}

Type *StartNode::compute() {
  auto i0 = dynamic_cast<TypeInteger *>(in(1)->type_);
  auto i1 = dynamic_cast<TypeInteger *>(in(2)->type_);
  if (i0->isConstant() && i1->isConstant()) {
    return TypeInteger::constant(i0->value() - i1->value());
  }
  return &Type::BOTTOM;
}

std::string StartNode::label() {
  return "Start";
}
Node *StartNode::idealize() { return nullptr; }
