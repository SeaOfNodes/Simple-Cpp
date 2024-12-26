#ifndef CAST_NODE_H
#define CAST_NODE_H

#include "node.h"
class CastNode : public Node {
public:
    Type* t;
    CastNode(Type* t, Node* ctrl, Node* in);
    std::string label() override;
    std::string uniqueName() override;

    std::ostringstream& print_1(std::ostringstream& builder, Tomi::Vector<bool>& visited) override;
    Type* compute() override;
    Node* idealize() override;

};
#endif