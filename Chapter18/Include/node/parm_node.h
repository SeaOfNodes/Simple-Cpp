#ifndef PARM_NODE_H
#define PARM_NODE_H

#include "node.h"
#include "mem_op_node.h"
#include "phi_node.h"

// forward declaration
class FunNode;

// Parameter Node
class ParmNode : public PhiNode {
public:
    // Argument indices are mapped one-to-one on CallNode inputs
    int idx_;    // Argument index


    ParmNode(std::string label, int idx, Type *declaredType, std::initializer_list<Node *> inputs);

    ParmNode(std::string label, int idx, Type *declaredType, Tomi::Vector<Node *> &inputs);

    std::string label() override;

    std::string glabel() override;

    FunNode *fun();

    std::ostringstream &print_1(std::ostringstream &builder, Tomi::Vector<bool> &visited) override;

    Node *idealize() override;

    bool inProgress();


};

#endif