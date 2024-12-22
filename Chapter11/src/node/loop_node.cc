#include "../../Include/node/region_node.h"
#include "../../Include/node/cproj_node.h"
#include "../../Include/node/if_node.h"
#include "../../Include/node/never_node.h"
#include "../../Include/node/return_node.h"
#include "../../Include/node/stop_node.h"
#include "../../Include/parser.h"

LoopNode::LoopNode(Node *entry) : RegionNode({nullptr, entry, nullptr}) {}

CFGNode *LoopNode::entry() { return cfg(1); }
CFGNode *LoopNode::back() { return cfg(2); }

std::string LoopNode::label() { return "Loop"; }
Type *LoopNode::compute() {
  return inProgress() ? Type::CONTROL() : entry()->type_;
}

Node *LoopNode::idealize() {
  return inProgress() ? nullptr : RegionNode::idealize();
}

int LoopNode::idepth() {return _idepth(1); }
CFGNode *LoopNode::idom() { return cfg(0); }

int LoopNode::loopDepth() {
    if(loopDepth_ != 0) return loopDepth_;
    loopDepth_ = entry()->loopDepth_ + 1;
    // One-time tag loop exists

    for(CFGNode* idom = back(); idom != this; idom = idom->idom()) {
        // Walk idom in loop, setting depth
        idom->loopDepth_ = loopDepth_;
        // Loop exit hits the CProj before the If, instead of jumping from
        // Region directly to If.
        if(auto* proj = dynamic_cast<CProjNode*>(idom)) {
            assert(dynamic_cast<IfNode*>(proj->in(0)));
            // Find the loop exit CProj, and set loop_depth
            for(Node* use: proj->in(0)->outputs) {
                if(auto* proj2 = dynamic_cast<CProjNode*>(use); proj2 != idom) {
                    proj2->loopDepth_ = loopDepth_ + 1;
                }
            }
        }
    }
    return loopDepth_;
}
void LoopNode::forceExit(StopNode *stop) {
    // Walk the backedge, then immediate dominator tree util we hit this
    // Loop again.  If we ever hit a CProj from an If (as opposed to
    // directly on the If) we found our exit.
    CFGNode* x = back();
    while(x != this) {
        if(dynamic_cast<CProjNode*>(x)) return;
        x = x->idom();
    }
    // Found a no-exit loop.  Insert an exit
    auto* iff = alloc.new_object<NeverNode>(back());
    for(Node* use: outputs) {
        if(auto* phi = dynamic_cast<PhiNode*>(use)) {
            iff->addDef(use);
        }
    }
    CProjNode*t = alloc.new_object<CProjNode>(iff, 0, "True");
    CProjNode*f = alloc.new_object<CProjNode>(iff, 1, "False");

    setDef(2, f);
    stop->addDef(alloc.new_object<ReturnNode>(t, Parser::ZERO(), nullptr));
}
