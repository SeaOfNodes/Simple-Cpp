#ifndef NEW_NODE_H
#define NEW_NODE_H

#include "node.h"
#include "../type/type_mem_ptr.h"

class NewNode : public Node {
public:
    NewNode(TypeMemPtr* ptr, Node* ctrl);
    TypeMemPtr* ptr_;
    std::string label() override;
    std::string glabel() override;
    std::ostringstream &print_1(std::ostringstream &builder, Tomi::Vector<bool>& visited) override;
    Type *compute() override;
    Node *idealize() override;
    bool eq(Node* n) override;
    int hash() override;
};
#endif