#ifndef DIVF_NODE_H
#define DIVF_NODE_H
#include "../../Include/node/node.h"

class DivFNode: public Node {
public:
    DivFNode(Node* lhs, Node* rhs);
    std::string label() override;
    std::string glabel() override;
    std::ostringstream &print_1(std::ostringstream &builder, Tomi::Vector<bool>&) override;
    Type *compute() override;
    Node *idealize() override;
    Node *copy(Node *lhs, Node *rhs) override;
};

#endif