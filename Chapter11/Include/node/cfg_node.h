#ifndef CFG_NODE_H
#define CFG_NODE_H

#include "node.h"

class CFGNode;

template<>
struct Tomi::hash<CFGNode*> {
    unsigned long long operator()(CFGNode *val);
};

class CFGNode : public Node {
public:
    CFGNode() = default;
    CFGNode(std::initializer_list<Node*> nodes);
    bool isCFG() override;

    bool isPinned() override;

    CFGNode *cfg(int idx);

    // Block head is Start, Region, CProj, but not e.g. If, Return, Stop
    virtual bool blockHead();

    // Should be exactly 1 tail from a block head
    CFGNode *blockTail();

    /**
 * Immediate dominator tree depth, used to approximate a real IDOM during
 * parsing where we do not have the whole program, and also peepholes
 * change the CFG incrementally.
 * <p>
 * See {@link <a href="https://en.wikipedia.org/wiki/Dominator_(graph_theory)">...</a>}
 */
    int idepth_{};

    virtual int idepth();

    // Return the immediate dominator of this Node and compute dom tree depth.
    virtual CFGNode *idom();

    // Return the LCA of two idoms
    virtual CFGNode *idom(CFGNode *rhs);

    int loopDepth_{};

    virtual int loopDepth();

    // Anti-dependence field support
    int anti_{};   // Per-CFG field to help find anti-deps
// ------------------------------------------------------------------------
// Support routines for Global Code Motion

// Tik-tok recursion pattern.  This method is final, and every caller does
// this work.
    virtual void walkUnreach(Tomi::BitArray<10> &visited, Tomi::HashSet<CFGNode *>& unreach);

    virtual void walkUnreach_(Tomi::BitArray<10> &vitisted, Tomi::HashSet<CFGNode *>& unreach);

};

#endif