#include "../../Include/node/region_node.h"

RegionNode::RegionNode(std::initializer_list<Node *> nodes) : Node(nodes) {}
std::string RegionNode::label() { return "Region"; }

std::ostringstream &RegionNode::print_1(std::ostringstream &builder) {
  builder << label();
  builder << nid;
  return builder;
}

bool RegionNode::isCFG() const { return true; }

Type *RegionNode::compute() { return &Type::CONTROL; }
Node *RegionNode::idealize() { return nullptr; }
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