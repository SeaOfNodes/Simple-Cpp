#include "../../Include/node/mul_node.h"
#include "../../Include/type/integer_type.h"
#include "../../Include/node/mulf_node.h"

MulNode::MulNode(Node *lhs, Node *rhs) : Node({nullptr, lhs, rhs}) {}

std::string MulNode::label() { return "Mul"; }
std::string MulNode::glabel() { return "*"; }

std::ostringstream &MulNode::print_1(std::ostringstream &builder,
                                     Tomi::Vector<bool> &visited) {
  builder << "(";
  in(1)->print_0(builder, visited);

  builder << "*";
  in(2)->print_0(builder, visited);

  builder << ")";

  return builder;
}

Type *MulNode::compute() {
    Type*t1 = in(1)->type_;
    Type*t2 = in(2)->type_;
    if(t1->isHigh() || t2->isHigh()) return TypeInteger::TOP();

  auto i1 = dynamic_cast<TypeInteger *>(t1);
  auto i2 = dynamic_cast<TypeInteger *>(t2);
  if (i1 && i2) {
      if(i1 == TypeInteger::ZERO() || i2 == TypeInteger::ZERO()) {
          return TypeInteger::ZERO();
      }
    if (i1->isConstant() && i2->isConstant()) {
      return TypeInteger::constant(i1->value() * i2->value());
    }
  }
  // Todo: understand why bot here
  return TypeInteger::BOT();
}

Node *MulNode::idealize() {
  Node *lhs = in(1);
  Node *rhs = in(2);
  Type *t1 = lhs->type_;
  Type *t2 = rhs->type_;

  auto i = dynamic_cast<TypeInteger *>(t2);

  // Mul of 1.  We do not check for (1*x) because this will already
  // canonicalize to (x*1)

  if (t2->isConstant() && i && i->value() == 1) {
    return lhs;
  }
  // Move constants to RHS: con*arg becomes arg*con
  if (t1->isConstant() && !t2->isConstant()) {
    return swap12();
  }
  // Do we have ((x * (phi cons)) * con) ?
  // Do we have ((x * (phi cons)) * (phi cons)) ?
  // Push constant up through the phi: x * (phi con0*con0 con1*con1...)
  Node *phicon = AddNode::phiCon(this, true);
  if (phicon != nullptr)
    return phicon;
  return nullptr;
}

Node *MulNode::copy(Node *lhs, Node *rhs) { return alloc.new_object<MulNode>(lhs, rhs); }
Node* MulNode::copyF() { return alloc.new_object<MulFNode>(nullptr, nullptr); }