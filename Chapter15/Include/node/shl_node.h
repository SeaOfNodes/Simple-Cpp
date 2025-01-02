#ifndef SHL_NODE_H
#define SHL_NODE_H
#include "node.h"

class ShlNode : public Node {
public:
    ShlNode(Node *lhs, Node *rhs);
    std::string label() override;
    std::string glabel() override;
    std::ostringstream &print_1(std::ostringstream &builder, Tomi::Vector<bool> &) override;
    Type *compute() override;
    Node *idealize() override;
    Node *copy(Node *lhs, Node *rhs) override;
    std::string err() override;
};
#endif