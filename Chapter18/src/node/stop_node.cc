#include "../../Include/node/stop_node.h"
#include "../../Include/globalCodeMotion.h"
#include "../../Include/parser.h"
#include <functional>

StopNode::StopNode(std::initializer_list<Node *> inputs) : CFGNode(inputs) {
    type_ = Type::BOTTOM();
}
std::string StopNode::label() { return "Stop"; }
std::ostringstream &StopNode::print_1(std::ostringstream &builder,
                                      Tomi::Vector<bool> &visited) {
    // For the sake of many old tests, and single value prints as "return val"

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

bool StopNode::blockHead() { return true; }
ReturnNode *StopNode::ret() {
    auto*ret = dynamic_cast<ReturnNode*>(in(0));
  return nIns() == 1 && ret ? ret: nullptr;
}

Type *StopNode::compute() { return Type::BOTTOM(); }


int StopNode::idepth() {
    if(idepth_ != 0) return idepth_;
    int d = 0;
    for(Node* n: inputs) {
        if(n != nullptr) {
            d = std::max(d, dynamic_cast<CFGNode*>(n)->idepth()+1);
        }
    }
    idepth_ = d;
    return idepth_;
}
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

