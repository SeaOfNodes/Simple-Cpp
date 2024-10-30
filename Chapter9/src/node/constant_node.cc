#include "../../Include/node/constant_node.h"

ConstantNode::ConstantNode(Type *type, Node *START)
    : con_(type), Node({START}) {}
std::ostringstream &ConstantNode::print_1(std::ostringstream &builder,
                                          std::vector<bool>& visited) {
  return con_->print_1(builder);
}

std::string ConstantNode::label() { return con_->print_1(builder).str(); }

Type *ConstantNode::compute() { return con_; }

bool ConstantNode::isMultiTail() { return true; }
std::string ConstantNode::uniqueName() { return "Con_" + std::to_string(nid); }
Node *ConstantNode::idealize() { return nullptr; }