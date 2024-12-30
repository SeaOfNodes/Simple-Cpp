#ifndef SHR_NODE_H
#define SHR_NODE_H
#include "node.h"
class ShrNode : public Node {
public:
    ShrNode(Node *lhs, Node *rhs);
    std::string label() override;
    std::string glabel() override;
    std::ostringstream &print_1(std::ostringstream &builder, Tomi::Vector<bool> &) override;
    Type *compute() override;
    Node *idealize() override;
    Node *copy(Node *lhs, Node *rhs) override;
};

#endif