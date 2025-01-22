#include "../../Include/node/cfg_node.h"
#include "../../Include/node/loop_node.h"
#include "../../Include/node/stop_node.h"
#include "../../Include/node/call_node.h"
#include "../../Include/node/fun_node.h"
#include "../../Include/node/call_end_node.h"
#include "../../Include/parser.h"

CFGNode::CFGNode(std::initializer_list<Node *> nodes) : Node(nodes) {
}

CFGNode::CFGNode(Tomi::Vector<Node *> nodes) : Node(nodes) {}

CFGNode *CFGNode::cfg(int idx) {
    return dynamic_cast<CFGNode *>(in(idx));
}

CFGNode *CFGNode::uctrl() {
    CFGNode *c = nullptr;
    for (Node *use: outputs) {
        if (auto *cfg = dynamic_cast<CFGNode *>(use)) {
            c = cfg;
        }
    }
    return c;
}

CFGNode::LoopTree::LoopTree(LoopNode *head) : head(head) {}

std::string CFGNode::LoopTree::ToString() { return "LOOP" + std::to_string(head->nid); }

unsigned long long Tomi::hash<CFGNode *>::operator()(CFGNode *val) {
    return val->hash();
}

bool CFGNode::skip(CFGNode *usecfg) {
    // Only walk control users that are alive.
    // Do not walk from a Call to linked Fun's.
    return dynamic_cast<XCtrlNode *>(usecfg) ||
           (dynamic_cast<CallNode *>(this) && dynamic_cast<FunNode *>(usecfg)) ||
           (dynamic_cast<ReturnNode *>(this) && dynamic_cast<CallEndNode *>(usecfg));
}

bool CFGNode::blockHead() {
    return false;
}

int CFGNode::idepth() {
    return idepth_ == 0 ? (idepth_ = idom()->idepth() + 1) : idepth_;
}

CFGNode *CFGNode::idom() {
    return idom(nullptr);
}


CFGNode *CFGNode::idom(CFGNode *rhs, Node *dep) {
    if (rhs == nullptr) return this;
    CFGNode *lhs = this; // this->nid: 76

    while (lhs != rhs) {
        int comp = lhs->idepth() - rhs->idepth();
        if (comp >= 0) lhs = dynamic_cast<CFGNode *>((lhs->addDep(dep)))->idom();
        if (comp <= 0) rhs = dynamic_cast<CFGNode *>((rhs->addDep(dep)))->idom();
    }
    return lhs;
}

CFGNode *CFGNode::idom(Node *dep) { return cfg(0); }

int CFGNode::loopDepth() {
    return ltree == nullptr ? 0 : ltree->depth();
}

LoopNode *CFGNode::loop() {
    return ltree->head;
}

void CFGNode::buildLoopTree(StopNode *stop) {
    ltree = stop->ltree = Parser::XCTRL->ltree = alloc.new_object<LoopTree>(dynamic_cast<StartNode *>(this));
    Tomi::BitArray<10> visited;
    bltWalk_(2, nullptr, stop, visited);
}

int CFGNode::LoopTree::depth() {
    return depth_ == 0 ? (par_ == nullptr ? 0 : (depth_ == par_->depth() + 1)) : depth_;
}

int CFGNode::bltWalk_(int pre, FunNode *fun, StopNode *stop, Tomi::BitArray<10> &post) {
    // Pre-walked?
    if (pre != 0) return pre;
    pre_ = pre++;
    // Pre-walk
    for (Node *use: outputs) {
        if (auto *usecfg = dynamic_cast<CFGNode *>(use); !skip(usecfg)) {
            auto *fuse = dynamic_cast<FunNode *>(use);
            auto *use = fuse ? fuse : fun;
            pre = usecfg->bltWalk_(pre, use, stop, post);
        }
    }
    // Post-order work: find innermost loop
    LoopTree *inner = nullptr;
    LoopTree *ltree = nullptr;

    for (Node *use: outputs) {
        auto *usecfg = dynamic_cast<CFGNode *>(use);
        if (!usecfg) continue;
        if (skip(usecfg)) continue;
        if (usecfg->type_ == Type::XCONTROL() || usecfg->type_ == TypeTuple::IF_NEITHER()) continue;
        // Child visited but not post-visited?
        if (!post.test(usecfg->nid)) {
            // Must be a backedge to a LoopNode then
            ltree = usecfg->ltree = alloc.new_object<LoopTree>(dynamic_cast<LoopNode *>(usecfg));

        } else {
            // Take child's loop choice, which must exist
            ltree = usecfg->ltree;
            // If falling into a loop, use the target loop's parent instead
            if (ltree->head == usecfg) {
                if (ltree->par_ == nullptr) {
                    // This loop never had an If test choose to take its
                    // exit, i.e. it is a no-exit infinite loop.
                    ltree->par_ = ltree->head->forceExit(fun, stop)->ltree;
                }
                ltree = ltree->par_;
            }
            // Sort inner loops.  The decision point is some branch far removed
            // from either loop head OR either backedge so requires pre-order
            // numbers to figure out innermost.
            if (inner == nullptr) {
                inner = ltree;
                continue;
            }
            if (inner == ltree) continue;
            LoopTree *outer = ltree->head->pre_ > inner->head->pre_ ? inner : ltree;
            inner = ltree->head->pre_ > inner->head->pre_ ? ltree : inner;
            inner->par_ = outer;
        }
    }
    if (inner != nullptr) {
        ltree = inner;
    }
    // Tag as post walked
    post.set(nid);
    return pre;
}