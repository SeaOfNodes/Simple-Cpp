#ifndef AND_NODE_H
#define AND_NODE_h
#include "../../Include/node/node.h"

class AndNode : public Node {
public:
    AndNode(Node *lhs, Node *rhs);

    std::string label() override;

    std::string glabel() override;

    std::ostringstream &print_1(std::ostringstream &builder,
                                Tomi::Vector<bool> &visited) override;

    Type *compute() override;

    Node *idealize() override;
    Node* copy(Node* lhs, Node* rhs) override;
    std::string err() override;
};
#endif