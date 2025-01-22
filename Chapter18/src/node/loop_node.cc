#include "../../Include/node/region_node.h"
#include "../../Include/node/cproj_node.h"
#include "../../Include/node/if_node.h"
#include "../../Include/node/never_node.h"
#include "../../Include/node/return_node.h"
#include "../../Include/node/stop_node.h"
#include "../../Include/parser.h"
#include "../../Include/node/fun_node.h"

LoopNode::LoopNode(Lexer *loc, Node *entry) : RegionNode(loc, {nullptr, entry, nullptr}) {}

CFGNode *LoopNode::entry() { return cfg(1); }

CFGNode *LoopNode::back() { return cfg(2); }

std::string LoopNode::label() { return "Loop"; }

Type *LoopNode::compute() {
    return inProgress() ? Type::CONTROL() : entry()->type_;
}

Node *LoopNode::idealize() {
    return inProgress() ? nullptr : RegionNode::idealize();
}

int LoopNode::idepth() { return idepth_ == 0 ? (idepth_ = CFGNode::idom()->idepth() + 1) : idepth_; }

CFGNode *LoopNode::idom(Node *dep) { return entry(); }

StopNode *LoopNode::forceExit(FunNode *fun, StopNode *stop) {
    // Walk the backedge, then immediate dominator tree util we hit this
    // Loop again.  If we ever hit a CProj from an If (as opposed to
    // directly on the If) we found our exit.
    CFGNode *x = back();
    while (x != this) {
        auto *exit = dynamic_cast<CProjNode *>(x);
        auto *iff = dynamic_cast<IfNode *>(exit->in(0));
        if (exit && iff) {
            CFGNode *other = iff->cproj(1 - exit->idx_);
            auto *ou = dynamic_cast<LoopNode *>(other->out(0));
            if (other->loopDepth() < loopDepth()) {
                return stop;
            }
        }
        x = x->idom();
    }
    // Found a no-exit loop.  Insert an exit
    auto *iff = alloc.new_object<NeverNode>(back());
    for (Node *use: outputs) {
        if (auto *phi = dynamic_cast<PhiNode *>(use)) {
            iff->addDef(use);
        }
    }
    CProjNode *t = alloc.new_object<CProjNode>(iff, 0, "True");
    CProjNode *f = alloc.new_object<CProjNode>(iff, 1, "False");

    setDef(2, f);


    // Now fold control into the exit.  Might have 1 valid exit, or an
    // XCtrl or a bunch of prior NeverNode exits.

    Node *top = alloc.new_object<ConstantNode>(Type::TOP(), Parser::START)->peephole();
    ReturnNode *ret = fun->ret();
    Node *ctrl = ret->ctrl();
    Node *mem_ = ret->mem();
    Node *expr = ret->expr();
    if (ctrl->type_ != Type::XCONTROL()) {
        // Check for perfect aligned exit
        auto r = dynamic_cast<RegionNode *>(ctrl);
        auto pmem = dynamic_cast<PhiNode *>(mem_);
        auto prez = dynamic_cast<PhiNode *>(expr);

        if (!r || !pmem || !prez || pmem->region() != r || prez->region() != r) {
            // Insert an aligned exit layer
            ctrl = alloc.new_object<RegionNode>(loc_, std::initializer_list<Node *>{nullptr, ctrl})->init<RegionNode>();

            //Todo: continue from here
            mem_ = alloc.new_object<PhiNode>(dynamic_cast<RegionNode *>(ctrl), mem_)->init<PhiNode>();
            expr = alloc.new_object<PhiNode>(dynamic_cast<RegionNode *>(ctrl), expr)->init<PhiNode>();
        }
        // Append new Never exit
        ctrl->addDef(t);
        mem_->addDef(top);
        expr->addDef(top);
    } else {
        ctrl = t;
        mem_ = top;
        expr = top;
    }
    ret->setDef(0, ctrl);
    ret->setDef(1, mem_);
    ret->setDef(2, expr);
    return stop;
}
