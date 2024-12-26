#include "../../Include/node/load_node.h"
#include "../../Include/node/store_node.h"
#include "../../Include/node/phi_node.h"
#include "../../Include/node/new_node.h"
#include "../../Include/node/cast_node.h"

LoadNode::LoadNode(std::string name, int alias, Type *glb, Node *memSlice, Node *memPtr) : MemOpNode(name, alias, {nullptr, memSlice, memPtr}), declaredType(glb) {}

std::string LoadNode::label() {return "Load";}
std::string LoadNode::glabel() {return "."+name_;}
std::ostringstream &LoadNode::print_1(std::ostringstream &builder, Tomi::Vector<bool> &visited) {
   builder << "." << name_;
    return builder;
}

Type *LoadNode::compute() {
    return declaredType;
}

// Todo: finish these
Node* LoadNode::idealize() {
    // Simple Load-after-Store on same address.
    auto st = dynamic_cast<StoreNode*>(mem());
    auto stm = dynamic_cast<PhiNode*>(mem());

    if(st && ptr() == st->ptr()) {
        return st->val();
    }
    if(stm && stm->region()->type_ == Type::CONTROL() && stm->nIns() == 3) {
        if(profit(stm, 2) || (!dynamic_cast<LoopNode*>(stm->region()) && profit(stm, 1))) {
            if (dynamic_cast<NewNode *>(ptr()) || !dynamic_cast<CastNode *>(ptr())) {
                if(!dynamic_cast<NewNode*>(ptr())) throw std::runtime_error("TODO");

                Node *ld1 = (new LoadNode(name_, alias_, declaredType, stm->in(1), ptr()))->peephole();
                Node *ld2 = (new LoadNode(name_, alias_, declaredType, stm->in(2), ptr()))->peephole();
                return new PhiNode(name_, type_, std::initializer_list < Node * > {stm->region(), ld1, ld2});
            }
        }
    }
    // Push a Load up through a Phi, as long as it collapses on at least
    // one arm.  If at a Loop, the backedge MUST collapse - else we risk
    // spinning the same transform around the loop indefinitely.
    //   BEFORE (2 Sts, 1 Ld):          AFTER (1 St, 0 Ld):
    //   if( pred ) ptr.x = e0;         val = pred ? e0
    //   else       ptr.x = e1;                    : e1;
    //   val = ptr.x;                   ptr.x = val;
    return nullptr;
}
bool LoadNode::profit(PhiNode* phi, int idx) {
    Node* px = phi->in(idx);
    if(px != nullptr) {
        auto* st1 = dynamic_cast<StoreNode*>(px->addDep(this));
        return (st1 && ptr() == st1->ptr());
    }
    return false;
}