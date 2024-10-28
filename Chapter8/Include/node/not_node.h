#ifndef NOT_NODE_H
#define NOT_NODE_H
#include "../../Include/node/node.h"

class NotNode : public Node {
public:
    explicit NotNode(Node *in);

    std::string label() override;

    std::string glabel() override;

    std::ostringstream &print_1(std::ostringstream &builder, std::vector<bool>& visited) override;

    Type *compute() override;

    Node *idealize() override;
};
#endif
