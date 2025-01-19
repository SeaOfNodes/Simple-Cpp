#ifndef AND_NODE_H
#define AND_NODE_H
#include "../../Include/node/node.h"
#include "../../Include/node/logical_node.h"

class AndNode : public LogicalNode {
public:
    AndNode(Lexer loc, Node *lhs, Node *rhs);

    std::string label() override;

    std::string op() override;
    std::string glabel() override;


    std::ostringstream &print_1(std::ostringstream &builder,
                                Tomi::Vector<bool> &visited) override;

    Type *compute() override;

    Node *idealize() override;
    Node* copy(Node* lhs, Node* rhs) override;
};
#endif