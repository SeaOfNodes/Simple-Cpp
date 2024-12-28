#ifndef NEVER_NODE_H
#define NEVER_NODE_H
#include "if_node.h"

class NeverNode : public IfNode {
public:
    NeverNode(Node* ctrl);
    std::string label() override;
    std::ostringstream &print_1(std::ostringstream &builder, Tomi::Vector<bool>& visited) override;
    Type* compute() override;
    Node* idealize() override;
};
#endif