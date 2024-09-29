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
