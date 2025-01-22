// CallEndNode first

#include "../../Include/node/call_node.h"
#include "../../Include/node/fun_node.h"
#include "../../Include/node/call_end_node.h"
#include "../../Include/type/type_fun_ptr.h"
#include "../../Include/codegen.h"
#include <bit>

CallNode::CallNode(Lexer *loc, std::initializer_list<Node *> inputs) : loc_(loc), CFGNode(inputs) {

}


CallNode::CallNode(Lexer *loc, Tomi::Vector<Node *> inputs) : loc_(loc), CFGNode(inputs) {

}

std::string CallNode::label() {
    return "Call";
}

std::ostringstream &CallNode::print_1(std::ostringstream &builder, Tomi::Vector<bool> &visited) {
    std::string fname;
    Node*fptr_ = fptr();
    if (auto *tfp = dynamic_cast<TypeFunPtr *>(fptr_->type_); tfp->isConstant()) {
        fname = tfp->name_;
    }
    if (fname.empty()) fptr_->print_0(builder, visited);
    else builder << fname;

    builder << "(";
    for (int i = 2; i < nIns() - 1; i++) {
        in(i)->print_0(builder, visited);
        if (i < nIns() - 2) { // Avoid trailing comma
            builder << ", ";
        }
    }
    // Todo: fence problem
    builder << ")";
    return builder;
}

Node *CallNode::ctrl() {
    return in(0);
}

Node *CallNode::mem() {
    return in(1);
}

Node *CallNode::arg(int idx) {
    return in(idx);
}

int CallNode::nargs() {
    return nIns() - 3;
}

Node *CallNode::fptr() {
    return inputs.back();
}

CallEndNode *CallNode::cend() {
    // Always in use slot 0
    auto *cend = dynamic_cast<CallEndNode *>(out(0));
    if (nOuts() > 0 && cend) {
        return cend;
    } else {
        return nullptr;
    }
}

CallEndNode *CallNode::cend_() {
    CallEndNode *cend = nullptr;
    for (Node *n: outputs) {
        if (auto *cend0 = dynamic_cast<CallEndNode *>(n)) {
            cend = cend0;
        }
    }
    return cend;
}

CFGNode *CallNode::uctrl() {
    return cend();
}

Type *CallNode::compute() {
    return ctrl()->type_;
}

Node *CallNode::idealize() {
    CallEndNode *cend_ = cend();
    if (cend_ == nullptr) return nullptr; // Still building
    // Link: call calls target function.  Linking makes the target FunNode
    // point to this Call, and all his Parms point to the call arguments;
    // also the CallEnd points to the Return.
    if (auto *tfp = dynamic_cast<TypeFunPtr *>(fptr()->type_); tfp && tfp->nargs() == nargs()) {
        // If fidxs is negative, then infinite unknown functions
        long fidxs = tfp->fidxs();
        if (fidxs > 0) {
            // Wipe out the return which matching in the linker table
            // Walk the (63 max) bits and link
            for (; fidxs != 0; fidxs = TypeFunPtr::nextFIDX(fidxs)) {
                int fidx = std::countr_zero(fidxs);
                TypeFunPtr *tfp0 = tfp->make_from(fidx);
                FunNode *fun = CodeGen::CODE->link(tfp0);
                if (fun != nullptr && !fun->folding_ && !linked(fun)) {
                    link(fun);
                }
            }
        }
    }
    return nullptr;
}

bool CallNode::linked(FunNode *fun) {
    for (Node *n: fun->inputs) {
        if (n == this) {
            return true;
        }
    }
    return false;
}

void CallNode::link(FunNode *fun) {
    fun->addDef(this);
    for (Node *use: fun->outputs) {
        if (auto *parm = dynamic_cast<ParmNode *>(use)) {
            parm->addDef(parm->idx_ == 0 ? cend() : arg(parm->idx_));

        }
    }
    // Call end points to function return
    IterPeeps::add(cend())->addDef(fun->ret());
}

// Parser exception error here