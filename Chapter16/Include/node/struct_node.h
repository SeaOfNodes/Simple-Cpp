#ifndef STRUCT_NODE_H
#define STRUCT_NODE_H
#include "node.h"
class TypeStruct;

class StructNode : public Node {
public:
    TypeStruct* ts_ = nullptr;
    std::string label() override;
    std::ostringstream &print_1(std::ostringstream &builder,
                                        Tomi::Vector<bool> &) override;
    Type* compute() override;

    Node* idealize() override;

    bool eq(Node* n) override;

    int hash() override;
};

#endif