#ifndef IF_NODE_H
#define IF_NODE_H
#include "node.h"
#include "multi_node.h"
#include "type/tuple_type.h"

class IfNode : public MultiNode {
    IfNode(Node*ctrl, Node* parent);
    std::string label() override;
    std::ostringstream& print_1(std::ostringstream& builder) override;

    bool isCFG();
    Node* ctrl();
    Node* pred();

    Type* compute() override;
    Node* idealize() override;
};
#endif