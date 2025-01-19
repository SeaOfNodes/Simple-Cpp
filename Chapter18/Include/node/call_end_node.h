#ifndef CALL_END_NODE_H
#define CALL_END_NODE_H

#include "cfg_node.h"

class CallNode; // forward decl

class CallEndNode : public CFGNode {
    // When set true, this Call/CallEnd/Fun/Return is being trivially inlined
    bool folding_;
public:
    CallEndNode(CallNode call);
    std::string label() override;
    bool isMultiHead() override;
    bool blockHead() override;
    CallNode* call();
    std::ostringstream& print_1(std::ostringstream& builder, Tomi::Vector<bool>& visited) override;
    Type*compute() override;
    Node* idealize() override;
    Node*pcopy(int idx);
};
#endif