#include "../../Include/node/phi_node.h"

PhiNode::PhiNode(std::string label, std::initializer_list<Node *> inputs)
    : Node(inputs), label_(label) {}
std::string PhiNode::label() { return "Phi_" + label_; }
std::string PhiNode::glabel() { return "&phi;_" + label_; }

std::ostringstream &PhiNode::print_1(std::ostringstream &builder,
                                     std::vector<bool>& visited) {
  if (dynamic_cast<RegionNode *>(region())->inProgress()) {
    builder << "Z";
  }
  builder << "Phi(";
  for (Node *in : inputs) {
    if (in == nullptr)
      builder << "___";
    else
      in->print_0(builder, visited);
    if (in != inputs.back())
      builder << ",";
  }
  builder << ")";
  return builder;
}
Node *PhiNode::region() { return in(0); }
Type *PhiNode::compute() {
  if (auto *r = dynamic_cast<RegionNode *>(region()); !r || r->inProgress())
    return &Type::BOTTOM;
  Type *t = &Type::TOP;
  for (int i = 1; i < nIns(); i++) {
    t = t->meet(in(i)->type_);
  }
  return t;
}

Node *PhiNode::singleUniqueInput() {
  Node *live = nullptr;
  for (int i = 1; i < nIns(); i++) {
    if (auto *loop = dynamic_cast<LoopNode *>(region());
        loop && (loop->entry()->type_ == &Type::XCONTROL))
      return nullptr;
    if (region()->in(i)->type_ != &Type::XCONTROL && in(i) != this) {
      if (live == nullptr || live == in(i)) {
        live = in(i);
      } else {
        return nullptr;
      }
    }
  }
  return live;
}
bool PhiNode::isMultiTail() { return true; }
Node *PhiNode::idealize() {
  if (auto *r = dynamic_cast<RegionNode *>(region()); !r || r->inProgress())
    return nullptr;
  // Remove a "junk" Phi: Phi(x,x) is just x
  Node *live = singleUniqueInput();
  if (live != nullptr)
    return live;

  /*  Node *op = in(1); // (arg+2)
    std::ostringstream b;
    std::cout << op->print_1(b).str() << "\n";
    std::cout << op->nIns() << "\n";
    std::cout << same_op() << "\n";*/

  // Pull "down" a common data op.  One less op in the world.  One more
  // Phi, but Phis do not make code.
  //   Phi(op(A,B),op(Q,R),op(X,Y)) becomes
  //   op(Phi(A,Q,X), Phi(B,R,Y)).

  Node *op = in(1);
  std::string label = op->label();
  if (op->nIns() == 3 && op->in(0) == nullptr && !op->isCFG() && same_op()) {
    std::vector<Node *> lhss(nIns());
    std::vector<Node *> rhss(nIns());
    lhss[0] = rhss[0] = in(0);
    // arg + 1, arg + 2
    // lhss - left hand side operands(region, arg, arg)
    // rhss - right hand side operands(region, 1, 2)
    for (int i = 1; i < nIns(); i++) {
      lhss[i] = in(i)->in(1);
      rhss[i] = in(i)->in(2);
    }
    Node *phi_lhs = alloc.new_object<PhiNode>(label_, lhss);
    Node *phi_rhs = alloc.new_object<PhiNode>(label_, rhss);
    // Phi(region, arg, arg)
    // Phi(region, 1, 2)
    // now the first one has the same inputs
    phi_lhs = phi_lhs->peephole();
    phi_rhs = phi_rhs->peephole();
    return op->copy(phi_lhs, phi_rhs);
  }
  return nullptr;
}
bool PhiNode::allCons() {
  if (auto *r = dynamic_cast<RegionNode *>(region()); !r || r->inProgress())
    return false;
  return Node::allCons();
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
