#ifndef RETURN_NODE_H
#define RETURN_NODE_H

#include "../../Include/node/node.h"
#include "../../Include/node/scope_node.h"

class ReturnNode : public CFGNode {
public:
    // ctrl - predecessor control node
    // data - Data node value
    ReturnNode(Node *ctrl, Node *mem, Node *data, Node *rpc, FunNode *fun);

    FunNode *fun_;

    Node *ctrl();

    Node *mem();

    Node *expr();

    std::ostringstream &print_1(std::ostringstream &builder, Tomi::Vector<bool> &visited) override;

    [[nodiscard]] bool isCFG() override;

    std::string label() override;

    Type *compute() override;

    Node *idealize() override;
};

#endif