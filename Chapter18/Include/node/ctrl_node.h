#ifndef CTRL_NODE_H
#define CTRL_NODE_H

#include "cfg_node.h"
class CtrlNode: CFGNode {
public:
    CtrlNode();
    std::string label() override;
    std::ostringstream print_1(std::ostringstream& os, Tomi::BitArray<10>& visited) override;
    bool isConst() override;
    Type* compute() override;
    Node* idealize() override;
};
#endif