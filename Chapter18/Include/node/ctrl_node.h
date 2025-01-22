#ifndef CTRL_NODE_H
#define CTRL_NODE_H

#include "cfg_node.h"
#include "../../Include/tomi.h"

class CtrlNode : public CFGNode {
public:
    CtrlNode();

    std::string label() override;

    std::ostringstream &print_1(std::ostringstream &os, Tomi::Vector<bool> &visited) override;

    bool isConst();

    Type *compute() override;

    Node *idealize() override;
};

#endif