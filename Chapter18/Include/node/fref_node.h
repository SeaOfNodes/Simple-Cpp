#ifndef FREF_NODE_H
#define FREF_NODE_H

#include "../../Include/node/constant_node.h"
#include "../../Include/node/scope_minnode.h"
class FRefNode : public ConstantNode {
public:
    static Type*FREF_TYPE;
    ScopeMinNode::Var* n_;
    FRefNode(ScopeMinNode::Var*n);

    std::string label() override;
    std::string uniqueName() override;

    std::ostringstream& print_1(std::ostringstream&os,  Tomi::Vector<bool>& visited) override;

    Node* idealize() override;

    // Todo: parser err comes here
    bool eq(Node*n) override;
    int hash() override;
};
#endif