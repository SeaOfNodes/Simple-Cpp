#ifndef ADDF_NODE_H
#define ADDF_NODE_H
#include "../../Include/node/node.h"

class AddFNode: public Node{
public:
    AddFNode(Node* lhs, Node* rhs);
    std::string label() override;
    std::string glabel() override;
    std::ostringstream &print_1(std::ostringstream &builder, Tomi::Vector<bool>&) override;
    Type *compute() override;
    Node *idealize() override;
    Node *copy(Node *lhs, Node *rhs) override;
};

#endif