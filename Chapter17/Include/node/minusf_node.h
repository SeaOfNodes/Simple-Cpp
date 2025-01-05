#ifndef MINUSF_NODE_H
#define MINUSF_NODE_H

#include "../../Include/node/node.h"

class MinusFNode: public Node {
public:
    MinusFNode(Node* in);
    std::string label() override;
    std::string glabel() override;
    std::ostringstream &print_1(std::ostringstream &builder, Tomi::Vector<bool>&) override;

    Type *compute() override;
    Node *idealize() override;
};
#endif