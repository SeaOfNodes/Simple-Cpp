#include "../../Include/node/bool_node.h"

BoolNode::BoolNode(Node *lhs, Node *rhs) : Node({nullptr, lhs, rhs}) {}

std::string BoolNode::label() { return "Bool Node"; }
std::string BoolNode::glabel() { return op(); }

std::string BoolNode::op() { return "BoolNode"; }
bool BoolNode::doOp(long lhs, long rhs) const { return false; }

std::ostringstream &BoolNode::print_1(std::ostringstream &builder,
                                      std::vector<bool> &visited) {
  builder << "(";
  in(1)->print_0(builder, visited);
  builder << op();
  in(2)->print_0(builder, visited);
  builder << ")";
  return builder;
}

Type *BoolNode::compute() {
  auto i0 = dynamic_cast<TypeInteger *>(in(1)->type_);
  auto i1 = dynamic_cast<TypeInteger *>(in(2)->type_);
  if (i0 && i1) {
    if (i0->isConstant() && i1->isConstant())
      return TypeInteger::constant(doOp(i0->value(), i1->value()) ? 1 : 0);
    return i0->meet(i1);
  }
  return &Type::BOTTOM;
}

Node *BoolNode::idealize() {
  // compare of Same(pointer comparison)
  if (in(1) == in(2))
    return alloc.new_object<ConstantNode>(
        TypeInteger::constant(doOp(3, 3) ? 1 : 0), Parser::START);
  // Do we have ((x * (phi cons)) * con) ?
  // Do we have ((x * (phi cons)) * (phi cons)) ?
  // Push constant up through the phi: x * (phi con0*con0 con1*con1...)

  Node *phicon = AddNode::phiCon(this, false);
  if (phicon != nullptr)
    return phicon;
  return nullptr;
}

// EQ
EQ::EQ(Node *lhs, Node *rhs) : BoolNode(lhs, rhs) {}

Node *EQ::copy(Node *lhs, Node *rhs) { return alloc.new_object<EQ>(lhs, rhs); }

std::string EQ::label() { return "EQ"; }
std::string EQ::op() { return "=="; }
bool EQ::doOp(long lhs, long rhs) const { return lhs == rhs; }

// LT
LT::LT(Node *lhs, Node *rhs) : BoolNode(lhs, rhs) {}

Node *LT::copy(Node *lhs, Node *rhs) { return alloc.new_object<LT>(lhs, rhs); }

std::string LT::label() { return "LT"; }

std::string LT::op() { return "<"; }
bool LT::doOp(long lhs, long rhs) const { return lhs < rhs; }

// LE
LE::LE(Node *lhs, Node *rhs) : BoolNode(lhs, rhs) {}

Node *LE::copy(Node *lhs, Node *rhs) { return alloc.new_object<LE>(lhs, rhs); }

std::string LE::label() { return "LE"; }

std::string LE::op() { return "<="; }
bool LE::doOp(long lhs, long rhs) const { return lhs <= rhs; }
