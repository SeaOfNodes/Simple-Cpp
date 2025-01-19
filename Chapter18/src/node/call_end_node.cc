#include "../../Include/node/call_end_node.h"
#include "../../Include/node/cfg_node.h"
#include "../../Include/node/return_node.h"
#include "../../Include/type/type_tuple.h"
#include "../../Include/type/type_mem.h"
#include "../../Include/type/type_fun_ptr.h"

CallEndNode::CallEndNode(CallNode*call): CFGNode({call}) {
    folding_ = false;
}

std::string CallEndNode::label() {
    return "CallEnd";
}

bool CallEndNode::isMultiHead() {
    return true;
}
bool CallEndNode::blockHead() {
    return true;
}

CallNode* CallEndNode::call() {
    return dynamic_cast<CallNode*>(in(0));
}

std::ostringstream &CallEndNode::print_1(std::ostringstream &builder, Tomi::Vector<bool> &visited) {
    builder << "cend( ";
    if(dynamic_cast<CallNode*>(in(0))) {
        builder << "Call, ";
    } else {
        builder << "----, ";
    }
    for(int i = 1; i < nIns(); i++) {
        in(1)->print_0(builder, visited);
        // Avoid trailing comma
        if(i < nIns()-1) {
            builder << ", ";
        }

    }
    builder << ")";
}

Type *CallEndNode::compute() {
    if(!dynamic_cast<CallNode*>(in(0))) {
        return TypeTuple::RET->dual();
    }
    auto*smt =  call()->fptr()->addDep(this)->type_;
    Type*ret;
    if(auto*tfp = dynamic_cast<TypeFunPtr*>(smt)) {
        ret = tfp->ret();
    } else {
        ret = Type::BOTTOM();
    }

    return TypeTuple::make(call->type_, TypeMem::BOT(), ret);
}
Node* CallEndNode::idealize() {

    // Trivial inlining: call site calls a single function; single function
    // is only called by this call site.
    if(!folding_ && nIns() == 2) {
        CallNode* call = call();
        Node*fptr = call->fptr();
        auto*tfp = dynamic_cast<TypeFunPtr*>(fptr->type_);
        if(fptr->nOuts() == 1 && // Only user is this call
        dynamic_cast<ConstantNode*>(fptr) && tfp && tfp->notNull() && call.err() == nullptr ){
            ReturnNode*ret = dynamic_cast<ReturnNode*>(in(1));
            FunNode* fun = ret->fun();
            // Expecting Start, and the call
            if(fun->nIns() == 3) {
                CFGNode*idom = call;
                while(!dynamic_cast<FunNode*>(idom)) {
                    idom = idom->idom();
                }
                if(idom != fun) {
                    // Trivial inline: rewrite
                    folding_= true;
                    // Rewrite Fun so the normal RegionNode ideal collapses
                    fun->folding_ = true;
                    fun->setDef(1, Parser::XCTRL()); // No default/ unknown StartNode earlier
                    fun->setDef(2, call->ctrl());  // Bypass the call
                    fun->ret()->setDef(3, nullptr); //Return is folding also
                    IterPeeps::addAll(fun->outputs_);
                    return this;
                }

            }
        }else {
            fun->addDep(this);
        }
    } else {
        // Function ptr has multiple users (so maybe multiple call sites)
        fptr->addDep(this);
    }
    return nullptr;
}

// why is this here?
Node* CallEndNode::pcopy(int idx) {
    return folding_ ? in(1)->in(idx) : nullptr;
}