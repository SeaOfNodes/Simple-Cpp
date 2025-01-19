#ifndef XCTRL_NODE_H
#define XCTRL_NODE_H
#include "../../Include/node/cfg_node.h"

class XCtrlNode : public CFGNode {
public:
    XCtrlNode();
    std::string label() override;
    bool isConst() override;
    bool isMultiTail() override;

    std::ostringstream &print_1(std::ostringstream &builder, Tomi::Vector<bool>& visited) override;
    Type* compute() override;
    Node* idealize() override;
};
#endif