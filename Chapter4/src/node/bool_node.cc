#include "../../Include/node/bool_node.h"
#include "../../Include/node/constant_node.h"
#include "../../Include/type/integer_type.h"

BoolNode::BoolNode(Node *lhs, Node *rhs) : Node({nullptr, lhs, rhs}) {}
std::string BoolNode::label() { return getClass().getSimpleName(); }
std::string BoolNode::glabel() { return op(); }

std::ostringstream &BoolNode::print_1(std::ostringstream &builder) {
  builder << "(";
  in(1)->print_1(builder);
  builder << op();
  in(2)->print_1(builder);
  builder << ")";
  return builder;
}

Type *BoolNode::compute() {
  auto i0 = dynamic_cast<TypeInteger *>(in(1)->type_);
  auto i1 = dynamic_cast<TypeInteger *>(in(2)->type_);
  if (i0 && i1) {
    if (i0->isConstant() && i1->isConstant())
      return TypeInteger::constant(doOp(i0().value(), i1.value()) ? 1 : 0);
    return i0->meet(i1);
  }
  return &Type::BOTTOM;
}
Node *BoolNode::idealize() {
  // compare of Same(pointer comparison)
  if (in(1) == in(2))
    return new ConstantNode(TypeInteger::constant(doOp(3, 3) ? 1 : 0));
  return nullptr;
}
// EQ
EQ::EQ(Node *lhs, Node *rhs) : Node(lhs, rhs) {}
std::string BoolNode::EQ::op() { return "=="; }
bool BoolNode::EQ::doOp(long lhs, long rhs) { return lhs == rhs; }

// LT
LT::LT(Node *lhs, Node *rhs) : Node(lhs, rhs) {}
std::string LT::op() { return "<"; }
bool LT::doOp(long lhs, long rhs) { return lhs < rhs; }

// LE
LE::LE(Node *lhs, Node *rhs) : Node(lhs, rhs) {}
std::string LE::op() { return "<="; }
bool LE::doOp(long lhs, long rhs) { return <= rhs; }