#include "../../Include/node/region_node.h"

LoopNode::LoopNode(Node *entry) : RegionNode({nullptr, entry, nullptr}) {}

Node *LoopNode::entry() { return in(1); }
Node *LoopNode::back() { return in(2); }

std::string LoopNode::label() { return "Loop"; }
Type *LoopNode::compute() {
  return inProgress() ? &Type::CONTROL : entry()->type_;
}

Node *LoopNode::idealize() {
  return inProgress() ? nullptr : RegionNode::idealize();
}

Node *LoopNode::idom() { return entry(); }