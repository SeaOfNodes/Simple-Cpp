#ifndef CALL_NODE_H
#define CALL_NODE_H

#include "cfg_node.h"
class Lexer;
class CallNode : public CFGNode {
    // Source location for late reported errors
public:
    Lexer loc_;
    CallNode(Lexer loc, std::initializer_list<Node*> inputs);
    CallNode(Lexer loc, std::vector<Node*> inputs);
    std::string label() override;
    std::ostringstream& print_1(std::ostringstream& builder, Tomi::Vector<bool>& visited) override;
    Node*ctrl();
    Node*mem();
    Node*arg(int idx);
    int nargs();
    Node* fptr();
    CallEndNode*cend();
    CallEndNode* cend_();
    CFGNode* uctrl() override;
    Type*compute() override;
    Node* idealize() override;
    bool linked(FunNode* fun);

    void link(FunNode* fun);

    // Parser exception error here

};
#endif