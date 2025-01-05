#ifndef SAR_NODE_H
#define SAR_NODE_H

#include "node.h"

class SarNode : public Node {
public:
    SarNode(Node *lhs, Node *rhs);
    std::string label() override;
    std::string glabel() override;
    std::ostringstream &print_1(std::ostringstream &builder, Tomi::Vector<bool> &) override;
    Type *compute() override;
    Node *idealize() override;
    Node *copy(Node *lhs, Node *rhs) override;
    std::string err() override;
};
#endif