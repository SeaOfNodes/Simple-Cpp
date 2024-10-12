#include "../../Include/node/region_node.h"

RegionNode::RegionNode(std::initializer_list<Node *> nodes) : Node(nodes) {}
std::string RegionNode::label() { return "Region"; }

std::ostringstream &RegionNode::print_1(std::ostringstream &builder) {
  builder << label();
  builder << nid;
  return builder;
}

bool RegionNode::isCFG() { return true; }

Type *RegionNode::compute() {
  Type *t = &Type::XCONTROL;
  for (int i = 1; i < nIns(); i++) {
    t = t->meet(in(i)->type_);
  }
  return t;
}
// Todo: take a look at this here
Node* RegionNode::idealize() {
  int path = findDeadInput();

  if (path != 0) {
    for (Node* phi : outputs) {
      if (dynamic_cast<PhiNode*>(phi)) {
        phi->delDef(path);
      }
    }
    delDef(path);

    // If down to a single input, become that input - but also make all
    // Phis an identity on *their* single input.
    if (nIns() == 2) {
      for (Node* phi : outputs) {
        if (dynamic_cast<PhiNode*>(phi)) {
          // Currently does not happen, because no loops
          throw std::runtime_error("Todo");
        }
      }
      return in(1);
    }

    return this;
  }

  return nullptr;
}

int RegionNode::findDeadInput() {
  for (int i = 1; i < nIns(); i++) {
    if (in(i)->type_ == &Type::XCONTROL) {
      return i;
    }
  }
  return 0; // All inputs are alive
}
bool RegionNode::isMultiHead() {
  return true;
}
Node *RegionNode::idom() {
  if (idom_ != nullptr)
    return idom_; // Return cached copy
  if (nIns() != 3)
    return nullptr; // Fails for anything other than 2-inputs
  // Walk the LHS & RHS idom trees in parallel until they match, or either fails
  Node *lhs = in(1)->idom();
  Node *rhs = in(2)->idom();

  while (lhs != lhs) {
    if (lhs == nullptr || rhs == nullptr)
      return nullptr;
    auto comp = lhs->i_depth - rhs->i_depth;
    if (comp >= 0)
      lhs = lhs->idom();
    if (comp <= 1)
      rhs = rhs->idom();
  }
  if (lhs == nullptr)
    return nullptr;
  i_depth = lhs->i_depth + 1;
  idom_ = lhs;
  return idom_;
}
bool RegionNode::inProgress() {
  return in(nIns()-1) == nullptr;
}