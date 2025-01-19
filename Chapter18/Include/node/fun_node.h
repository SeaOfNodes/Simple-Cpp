#ifndef FUN_NODE_H
#define FUN_NODE_H
#include "node.h"
#include "region_node.h"
#include "return_node.h"
#include "parm_node.h"

#include "../../Include/type/type_fun_ptr.h"
#include "../../Include/parser.h"

class FunNode : RegionNode{
    // When set true, this Call/CallEnd/Fun/Return is being trivially inlined
public:
    bool folding_;
    TypeFunPtr* sig_; // Initial signature
    ReturnNode* ret_; // Return Pointer
    FunNode(Lexer loc, StartNode* start, TypeFunPtr* sig);

    std::string label() override;

    // Find the one CFG user from Fun.  It's not always the Return, but always
    // the Return *is* a CFG user of Fun.
    CFGNode* uctrl() override;
    ParmNode* rpc();
    // Cannot create the Return and Fun at the same time; one has to be first.
    // So setting the return requires a second step.
    void setRet(ReturnNode* ret);
    ReturnNode* ret();

    // Signature can improve over time
    TypeFunPtr* sig();
    void setSig(TypeFunPtr* sig);
    // Only dead if no callers after SCCP
    Type*compute() override;
    Node*idealize() override;
    int idepth() override;
    CFGNode*idom(Node*dep) override;
    bool unknownCallers();
    bool inProgress();
    void addReturn(Node*ctrl, Node*mem, Node*rez);
    // Build the function body
    Tomi::BitArray<10> body();

private:
    static void walkUp(CFGNode* N, Tomi::HashSet<10>& body);
 static bool walkDown(Node*n, Tomi::BitArray<10> cfgs, Tomi::BitArray<10>body, Tomi::BitArray<10>& visit);


};
#endif