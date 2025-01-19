#include "../../Include/node/fun_node.h"

FunNode::FunNode(Lexer loc, StartNode *start, TypeFunPtr *sig) : RegionNode(loc, {nullptr, start}), sig_(sig) {
}

std::string FunNode::label() {
    return sig_->name_ == nullptr ? "$fun" : sig_->name_;
}

CFGNode *FunNode::uctrl() {
    for (Node *n: outputs) {
        if (dynamic_cast<CFGNode *>(n) && (dynamic_cast<RegionNode *>(cfg) || cfg->cfg0() == this)) {
            return cfg;
        }
    }
    return nullptr;
}

ParmNode *FunNode::rpc() {
    ParmNode *rpc = nullptr;
    for (Node *n: outputs) {
        if (auto *parm = dynamic_cast<ParmNode *>(n); param->idx_ == 0) {
            assert(rpc == nullptr);
            rpc = parm;
        }
    }
    return rpc;
}

void FunNode::setRef(ReturnNode* ret) {
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
    if(prog != nullptr) {
        IterPeeps::add(CodeGen::CODE->stop_);
        IterPeeps::add(ret_);
    }
    return progresss;
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
    addReturn(ret_,ctrl, mem, rez);
}

Tomi::BitArray<10> FunNode::body() {
    // Reverse up (stop to start) CFG only, collect bitmap.
    Tomi::BitArray<10> cfgs;
    cfgs->set(nid);
    walkUp(ret(), cfgs);

    // Top down (start to stop) all flavors.  CFG limit to bitmap.
    // If data use bottoms out in wrong CFG, returns false - but tries all outputs.
    // If any output hits an in-CFG use (e.g. phi), then keep node.

    Tomi::BitArray<10> body;
    walkDown(this, cfgs, body, Tomi::BitArray<10>());
    return body;
}
static void TypeFunPtr::walkUp(CFGNode* n, Tomi::BitArray<10>& cfgs) {
    if(cfgs.get(n->nid_)) return;
    cfgs.set(n->nid_);
    if(auto*r = dynamic_cast<RegionNode*>(n)) {
        for(int i = 1; i < n->nIns(); i++) {
            walkUp(n->cfg(i), cfgs);
        }
    } else walkUp(n->cfg0(), cfgs);
}
static bool TypeFunPtr::walkDown(Node*n, Tomi::BitArray<10> cfgs, Tomi::BitArray<10>body, Tomi::BitArray<10>& visit){
    if(visit.get(n->nid_)) return body.get(n->nid_);
    visit.set(n->nid_);

    if(dynamic_cast<CFGNode*>(n) && !cfgs.get(n->nid_)) return false;
    if(n->in(0) != nullptr && !cfgs.get(n->in(0)->nid_)) return false;

    bool in = n->in(0) != nullptr || dynamic_cast<CFGNode*>(n);
    for(Node* use: n->outputs) {
        in |= walkDown(use, cfgs, body, visit);

    }
    if(in) body.set(n->nid_);
    return in;
}
