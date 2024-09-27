#include "../../Include/node/phi_node.h"

PhiNode::PhiNode(std::string label, std::initializer_list<Node *> inputs)
    : Node(inputs), label_(label) {}
std::string PhiNode::label() { return "Phi_" + label_; }
std::string PhiNode::glabel() { return "&phi;_" + label_; }

std::ostringstream &PhiNode::print_1(std::ostringstream &builder) {
  builder << "Phi(";
  for (Node *in : inputs) {
    in->print_1(builder);
    if (in != inputs.back())
      builder << ",";
  }
  return builder;
}
Node *PhiNode::region() { return in(0); }
Type *PhiNode::compute() { return &Type::BOTTOM; }
Node *PhiNode::idealize() {
  // Remove a "junk" Phi: Phi(x,x) is just x
  if (same_inputs())
    return in(1);

  // Pull "down" a common data op.  One less op in the world.  One more
  // Phi, but Phis do not make code.
  //   Phi(op(A,B),op(Q,R),op(X,Y)) becomes
  //   op(Phi(A,Q,X), Phi(B,R,Y)).

  Node *op = in(1);
  if (op->nIns() == 3 && op->in(0) == nullptr && !op->isCFG() && same_op()) {
    std::vector<Node *> lhss(nIns());
    std::vector<Node *> rhss(nIns());
    lhss[0] = rhss[0] = in(0);
    for (int i = 1; i < nIns(); i++) {
      lhss[i] = in(i)->in(1);
      rhss[i] = in(i)->in(2);
    }
    Node *phi_lhs = new PhiNode(label_, lhss);
    Node *phi_rhs = new PhiNode(label_, rhss);
    phi_lhs->peephole();
    phi_rhs->peephole();
    return op->copy(phi_lhs, phi_rhs);
  }
  return nullptr;
}

PhiNode::PhiNode(std::string label, std::vector<Node *> inputs)
    : Node(inputs), label_(label) {}
bool PhiNode::same_op() {
  for (int i = 2; i < nIns(); i++) {
    if (typeid(*in(1)) != typeid(*in(i)))
      return false;
  }
  return true;
}
bool PhiNode::same_inputs() {
  for (int i = 2; i < nIns(); i++) {
    if (in(1) != in(i))
      return false;
  }
  return true;
}