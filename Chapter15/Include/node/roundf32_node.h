#ifndef ROUND_F32_NODE_H
#define ROUND_F32_NODE_H

#include "node.h"

class RoundF32Node : public Node {
public:
    RoundF32Node(Node *lhs);
    std::string label() override;
    std::string glabel() override;
    std::ostringstream &print_1(std::ostringstream &builder, Tomi::Vector<bool> &) override;
    Type *compute() override;
    Node *idealize() override;
};
#endif