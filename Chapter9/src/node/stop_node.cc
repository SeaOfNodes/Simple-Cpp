#include "../../Include/node/stop_node.h"

StopNode::StopNode(std::initializer_list<Node *> inputs) : Node(inputs) {}
std::string StopNode::label() { return "Stop"; }
std::ostringstream &StopNode::print_1(std::ostringstream &builder,
                                      Tomi::Vector<bool> &visited) {
  if (ret() != nullptr) {
    return ret()->print_0(builder, visited);
  }
  builder << "Stop[ ";
  for (Node *ret : inputs) {

    ret->print_0(builder, visited);
    builder << " ";
  }
  builder << "]";
  return builder;
}

StopNode *StopNode::iterate() { return IterPeeps::iterate(this, false); }
StopNode *StopNode::iterate(bool show) {
  return IterPeeps::iterate(this, show);
}

bool StopNode::isCFG() { return true; }
ReturnNode *StopNode::ret() {
  return nIns() == 1 ? (ReturnNode *)(in(0)) : nullptr;
}

Type *StopNode::compute() { return Type::BOTTOM(); }

Node *StopNode::idealize() {
  int len = static_cast<int>(nIns());
  // never got here
  for (int i = 0; i < nIns(); i++) {
    std::ostringstream b;
    if (!in(i)->type_)
      std::cout << "Type is not set";
    if (in(i)->type_ == Type::XCONTROL()) {
      delDef(i--);
    }
  }
  if (len != nIns())
    return this;
  return nullptr;
}

Node *StopNode::addReturn(Node *node) { return addDef(node); }