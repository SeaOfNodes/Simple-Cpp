#include "../../Include/node/cfg_node.h"

CFGNode::CFGNode(std::initializer_list<Node*> nodes) : Node(nodes) {
}

bool CFGNode::isCFG() {return true;}
bool CFGNode::isPinned() {return true;}

CFGNode* CFGNode::cfg(int idx) {
    return dynamic_cast<CFGNode*>(in(idx));
}
bool CFGNode::blockHead() {
    return false;
}
CFGNode* CFGNode::blockTail() {
    assert(blockHead());
    for(Node* n : inputs) {
        if(dynamic_cast<CFGNode*>(n) != nullptr) {
            return dynamic_cast<CFGNode*>(n);
        }
    }
    return nullptr;
}
int CFGNode::idepth() {
    return idepth_ == 0 ? (idepth_ = idom()->idepth() + 1) : idepth_;
}
CFGNode* CFGNode::idom() {
    return cfg(0);
}

CFGNode* CFGNode::idom(CFGNode* rhs) {
    if(rhs == nullptr) return this;
    CFGNode* lhs = this;
    while(lhs != rhs) {
        int comp = lhs->idepth() - rhs->idepth();
        if(comp >= 0) lhs = lhs->idom();
        if(comp <= 0) rhs = rhs->idom();
    }
    return lhs;
}

int CFGNode::loopDepth() {
    return loopDepth_ == 0? (loopDepth_ = cfg(0)->loopDepth()) : loopDepth_;
}
void CFGNode::walkUnreach(Tomi::BitArray<10> &visited, Tomi::HashSet<CFGNode *> unreach) {
    if(visited.test(nid)) return;
    visited.set(nid);
    walkUnreach_(visited, unreach);
    unreach.remove(this);
}

void CFGNode::walkUnreach_(Tomi::BitArray<10> &visited, Tomi::HashSet<CFGNode *> unreach) {
    cfg(0)->walkUnreach(visited, unreach);
}