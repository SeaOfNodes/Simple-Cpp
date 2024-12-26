#ifndef TO_FLOAT_NODE_H
#define TO_FLOAT_NODE_H

#include "../../Include/node/node.h"
#include "../../Include/type/integer_type.h"

class ToFloatNode : public Node {
public:
    explicit ToFloatNode(Node* lhs);
    std::string label() override;
    std::string glabel() override;
    std::ostringstream &print_1(std::ostringstream &builder, Tomi::Vector<bool>&) override;

    Type *compute() override;
    Node *idealize() override;

    Node *copy(Node *lhs);
};
#endif