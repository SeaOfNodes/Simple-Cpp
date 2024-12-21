#ifndef GLOBAL_CODE_H
#define GLOBAL_CODE_H
#include "../Include/node/stop_node.h"
#include "../Include/node/start_node.h"
#include "../Include/node/load_node.h"

#include "../Include/tomi.h"
class GlobalCodeMotion {
public:
    // Arrange that the existing isCFG() Nodes form a valid CFG.  The
    // Node.use(0) is always a block tail (either IfNode or head of the
    // following block).  There are no unreachable infinite loops.
    static void buildCFG(StopNode* stop);
    // ------------------------------------------------------------------------
    // Backwards walk on the CFG only, looking for unreachable code - which has
    // to be an infinite loop.  Insert a bogus never-taken exit to Stop, so the
    // loop becomes reachable.  Also, set loop nesting depth
    static void fixLoops(StopNode* stop);

    // Forwards walk over previously unreachable, looking for loops with no
    // exit test.
    static void walkInfinite(CFGNode* n, Tomi::BitArray<10>& visited, StopNode* stop);

    static void schedEarly();
    static void rpo_cfg(Node* n, Tomi::BitArray<10>& visited, Tomi::Vector<CFGNode*>& rpo);
    static void schedEarly_(Node* n, Tomi::BitArray<10>& visit);
    static void schedLate(StartNode* start);
    // Forwards post-order pass.  Schedule all outputs first, then draw an
    // idom-tree line from the LCA of uses to the early schedule.  Schedule is
    // legal anywhere on this line; pick the most control-dependent (largest
    // idepth) in the shallowest loop nest.
    static void schedLate_(Node* n, Tomi::Vector<Node*>& ns, Tomi::Vector<CFGNode*>& late);

    // Block of use.  Normally from late[] schedule, except for Phis, which go
    // to the matching Region input.
    static CFGNode* use_block(Node* n, Node* use, Tomi::Vector<CFGNode*>&late);
    // Least loop depth first, then largest idepth
    static bool better(CFGNode* a, CFGNode* best);

    // Skip iteration if a backedge
    static bool isForwardsEdge(Node* use, Node* def);

    static CFGNode* find_anti_dep(CFGNode* lca, LoadNode* load, CFGNode* early, Tomi::Vector<CFGNode*> late);
    static CFGNode* anti_dep(LoadNode* load, CFGNode* stblk, CFGNode* defblk, CFGNode* lca, Node* st);

};
#endif