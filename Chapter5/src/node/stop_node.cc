#include "../../Include/node/stop_node.h"

StopNode::StopNode(std::initializer_list<Node *> inputs) : Node(inputs) {}
std::string StopNode::label() { return "Stop"; }
std::ostringstream &StopNode::print_1(std::ostringstream &builder) {
  if (ret() != nullptr)
    return ret()->print_1(builder);
  builder << "Stop[";
  for (Node *ret : inputs) {
    ret->print_1(builder);
    builder << " ";
  }
  builder << "]";
  return builder;
}

bool StopNode::isCFG() const { return true; }
ReturnNode *StopNode::ret() {
  return nIns() == 1 ? (ReturnNode *)(in(0)) : nullptr;
}

Type *StopNode::compute() { return &Type::BOTTOM; }

Node *StopNode::idealize() { return nullptr; }

Node *StopNode::addReturn(Node *node) { return addDef(node); }