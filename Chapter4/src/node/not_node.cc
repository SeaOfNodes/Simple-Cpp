#include "../../Include/node/not_node.h"
#include "../../Include/type/integer_type.h"

NotNode::NotNode(Node in) : Node(nullptr, in) {}
std::string NotNode::label() { return "Not"; }

std::string NotNode::glabel() { return "!"; }
std::ostringstream &NotNode::print_1(std::ostringstream &builder) {
  builder << "(!";
  builder << in(0)->print_1(builder);
  builder << ")";
  return builder;
}

Type *NotNode::compute() {
  auto i = dynamic_cast<TypeInteger *>(in(1)->type_);
  if (i) {
    return i->isConstant() ? TypeInteger::constant(i->value() == 0 ? 1 : 0) : i;
  }
  return &Type::BOTTOM;
}

Type *NotNode::idealize() { return nullptr; }