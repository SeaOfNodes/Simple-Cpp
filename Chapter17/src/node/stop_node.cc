#include "../../Include/node/stop_node.h"
#include "../../Include/globalCodeMotion.h"
#include <functional>

StopNode *StopNode::GCM(bool show) {
    GlobalCodeMotion::fixLoops(this);
    GlobalCodeMotion::buildCFG(this);
    return this;
}
StopNode::StopNode(std::initializer_list<Node *> inputs) : CFGNode(inputs) {}
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

StopNode *StopNode::iterate() { return IterPeeps::iterate(this, false)->typeCheck()->GCM(false); }
StopNode *StopNode::iterate(bool show) {
  return IterPeeps::iterate(this, show)->typeCheck()->GCM(show);
}

bool StopNode::blockHead() { return true; }
ReturnNode *StopNode::ret() {
  return nIns() == 1 ? (ReturnNode *)(in(0)) : nullptr;
}

int StopNode::loopDepth() {
    return (loopDepth_=1);
}
Node* StopNode::getBlockStart() {
    return this;
}
CFGNode* StopNode::idom(Node* dep) { return nullptr; }

Type *StopNode::compute() { return Type::BOTTOM(); }

StopNode* StopNode::typeCheck() {
    std::function<std::string(Node*)> boundPrint = [](Node* n) {
        return n->err();
    };

    std::string err1 = walk(boundPrint);
    if(!err1.empty()) {
        throw std::runtime_error(err1);
    }
    return this;
}

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

Node *StopNode::addReturn(Node *node) {
    // TOdo: do smt here
    return addDef(node);
}