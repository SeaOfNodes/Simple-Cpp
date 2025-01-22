#include "../../Include/node/fun_node.h"
#include "../../Include/codegen.h"
FunNode::FunNode(Lexer* loc, StartNode *start, TypeFunPtr *sig) : RegionNode(loc, {nullptr, start}), sig_(sig) {
}

std::string FunNode::label() {
    return sig_->name_ == "" ? "$fun" : sig_->name_;
}

CFGNode *FunNode::uctrl() {
    for (Node *n: outputs) {
        auto*cfg = dynamic_cast<CFGNode *>(n);
        if (cfg && (dynamic_cast<RegionNode *>(cfg) || cfg->cfg0() == this)) {
            return cfg;
        }
    }
    return nullptr;
}

ParmNode *FunNode::rpc() {
    ParmNode *rpc = nullptr;
    for (Node *n: outputs) {
        if (auto *parm = dynamic_cast<ParmNode *>(n); parm->idx_ == 0) {
            assert(rpc == nullptr);
            rpc = parm;
        }
    }
    return rpc;
}

void FunNode::setRet(ReturnNode* ret) {
    ret_ = ret;
}
ReturnNode* FunNode::ret() {
    assert(ret_ != nullptr);
    return ret_;
}

TypeFunPtr*FunNode::sig() {
    return sig_;
}
void FunNode::setSig(TypeFunPtr *sig) {
    if(sig_ != sig) {
        IterPeeps::add(this);
    }
    sig_ = sig;
}

Type *FunNode::compute() {
    return Type::CONTROL();
}

Node*FunNode::idealize() {
    // When can we assume no callers?  Or no other callers (except main)?
    // In a partial compilation, we assume Start gets access to any/all
    // top-level public structures and recursively what they point to.
    // This in turn is valid arguments to every callable function.
    //
    // In a total compilation, we can start from Start and keep things
    // more contained.


    // If no default/unknown caller, use the normal RegionNode ideal rules
    // to collapse
    if(unknownCallers()) return nullptr;
    Node*progress = RegionNode::idealize();
    if(progress != nullptr) {
        IterPeeps::add(CodeGen::CODE->stop_);
        IterPeeps::add(ret_);
    }
    return progress;
}

int FunNode::idepth() {
    return (idepth_ =1);
}

CFGNode* FunNode::idom(Node*dep) {
    return cfg(1);
}
bool FunNode::unknownCallers() {
    if(dynamic_cast<StartNode*>(in(1))) {
        return true;
    }
    return false;
}

bool FunNode::inProgress() {
    return unknownCallers();
}

void FunNode::addReturn(Node *ctrl, Node *mem, Node *rez) {
    addReturn(ctrl, mem, rez);
}

Tomi::BitArray<10> FunNode::body() {
    // Reverse up (stop to start) CFG only, collect bitmap.
    Tomi::BitArray<10> cfgs;
    cfgs.set(nid);
    walkUp(ret(), cfgs);

    // Top down (start to stop) all flavors.  CFG limit to bitmap.
    // If data use bottoms out in wrong CFG, returns false - but tries all outputs.
    // If any output hits an in-CFG use (e.g. phi), then keep node.

    Tomi::BitArray<10> body;
    Tomi::BitArray<10> visit;
    walkDown(this, cfgs, body, visit);
    return body;
}
void FunNode::walkUp(CFGNode* n, Tomi::BitArray<10>& cfgs) {
    if(cfgs.test(n->nid)) return;
    cfgs.set(n->nid);
    if(auto*r = dynamic_cast<RegionNode*>(n)) {
        for(int i = 1; i < n->nIns(); i++) {
            walkUp(n->cfg(i), cfgs);
        }
    } else walkUp(n->cfg0(), cfgs);
}
bool FunNode::walkDown(Node*n, Tomi::BitArray<10> cfgs, Tomi::BitArray<10>body, Tomi::BitArray<10>& visit){
    if(visit.test(n->nid)) return body.test(n->nid);
    visit.set(n->nid);

    if(dynamic_cast<CFGNode*>(n) && !cfgs.test(n->nid)) return false;
    if(n->in(0) != nullptr && !cfgs.test(n->in(0)->nid)) return false;

    bool in = n->in(0) != nullptr || dynamic_cast<CFGNode*>(n);
    for(Node* use: n->outputs) {
        in |= walkDown(use, cfgs, body, visit);

    }
    if(in) body.set(n->nid);
    return in;
}
