#ifndef SAR_NODE_H
#define SAR_NODE_H

#include "node.h"
#include "logical_node.h"

class SarNode : public LogicalNode {
public:
    SarNode(Lexer *loc, Node *lhs, Node *rhs);

    std::string label() override;

    std::string glabel() override;

    std::string op() override;

    std::ostringstream &print_1(std::ostringstream &builder, Tomi::Vector<bool> &) override;

    Type *compute() override;

    Node *idealize() override;

    Node *copy(Node *lhs, Node *rhs) override;

    std::string err() override;
};

#endif