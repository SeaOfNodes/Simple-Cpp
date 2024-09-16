#include "../../Include/node/constant_node.h"

ConstantNode::ConstantNode(Type* type, Node* START) : con_(type), Node({START}){}
std::ostringstream& ConstantNode::print_1(std::ostringstream& builder) {
  return con_->_print(builder);
}

std::string ConstantNode::label() {
  return "#"+(con_->_print(builder).str());
}

Type *ConstantNode::compute() {
  return con_;
}

std::string ConstantNode::uniqueName() {
  return "Con_" + std::to_string(nid);
}
Node* ConstantNode::idealize() {return nullptr;}