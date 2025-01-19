#include "../../Include/node/load_node.h"
#include "../../Include/node/store_node.h"
#include "../../Include/node/phi_node.h"
#include "../../Include/node/new_node.h"
#include "../../Include/node/cast_node.h"
#include "../../Include/node/proj_node.h"

#include "../../Include/type/type_mem.h"
#include "../../Include/parser.h"
#include "node/read_only_node.h"

LoadNode::LoadNode(std::string name, int alias, Type *glb, Node *mem_, Node *ptr, Node*off_) : MemOpNode(name, alias, glb, mem_, ptr, off_){}

std::string LoadNode::label() {return "ld_" + mlabel();}
std::string LoadNode::glabel() {return "."+name_;}
std::ostringstream &LoadNode::print_1(std::ostringstream &builder, Tomi::Vector<bool> &visited) {
   builder << "." << name_;
    return builder;
}

Type *LoadNode::compute() {
    // Todo: understand this
    auto*mem1 = dynamic_cast<TypeMem*>(mem()->type_);
    if(mem1) {
        auto*tmp = dynamic_cast<TypeMemPtr*>(mem1->t_);
        if(tmp && declaredType->isFRef() &&  !tmp->isFRef()) {
            declaredType = tmp;
        }
            if(MemOpNode::err().empty()) return declaredType->join(mem1->t_);
    }
    return declaredType;
}

// Todo: understand this
Node* LoadNode::idealize() {
    // Simple Load-after-Store on same address.
    if(nid == 28) {
        std::cerr << "Here";
    }
    Node* ptr1 = ptr();
    auto st = dynamic_cast<StoreNode*>(mem());
    auto stm = dynamic_cast<PhiNode*>(mem());

    if(st && ptr() == st->ptr() && off() == st->off()) {
        return st->val();
    }

    // Simple Load-after-New on same address.
    auto*p = dynamic_cast<ProjNode*>(mem());
    if(p && dynamic_cast<NewNode*>(p->in(0)) && ptr1 == dynamic_cast<NewNode*>(p->in(0))->proj(1)) {
        auto*nnn = dynamic_cast<NewNode*>(p->in(0));
        // nnn->find_alias(alias_) should return ReadOnlyNode
        return nnn->in(nnn->find_alias(alias_));
    }

    // Load-after-Store on same address, but bypassing provably unrelated
    // stores.  This is a more complex superset of the above two peeps.
    // "Provably unrelated" is really weak.
    auto*ro = dynamic_cast<ReadOnlyNode*>(ptr1);
    if(ro) {
        ptr1 =ro->in(1);
    }
    if(nid == 49) {
        std::cerr << "Here";
    }
    Node*mem1 = mem();
    while(true) {
       if(auto*st_ = dynamic_cast<StoreNode*>(mem1)) {
           if(ptr1 == st_->ptr() && off() == st_->off()) {
               return castR0(st->val());
           }
           // Can we prove unequal?  Offsets do not overlap?
           if(!st_->off()->type_) {
               // error here // Todo: fix
               std::cerr << "Here";
           }
           if(!st_->off()->type_) {
               std::cerr << "Here";
           }
           if(!off()->type_->join(st_->off()->type_)->isHigh() && !neverAlias(ptr1, st_->ptr())){
               break;
           }
           mem1 = st->mem();
           continue;
       }
    if(auto*phi = dynamic_cast<PhiNode*>(mem1)) {
        break;
    }
    if(auto*mproj = dynamic_cast<ProjNode*>(mem1)) {
        if(auto*nnn1 =dynamic_cast<NewNode*>(mproj->in(0))) {
            if(auto*pproj = dynamic_cast<ProjNode*>(ptr1); pproj->in(0) == mproj->in(0)) {
                return castR0(nnn1->in(nnn1->find_alias(alias_)));
            }
            if(auto*pproj = dynamic_cast<ProjNode*>(ptr1); !pproj && !dynamic_cast<NewNode*>(pproj->in(0))) {
                break;
            }
            mem1 = nnn1->in(alias_);
            continue;
        } else if (dynamic_cast<StartNode*>(mproj->in(0)) ) {
            break;
        }
        else {
            throw std::runtime_error("TODO!");
        }
    }
    int id = nid;
    throw std::runtime_error("TODO!");
    }
    auto*memphi = dynamic_cast<PhiNode*>(mem());
    if(stm && stm->region()->type_ == Type::CONTROL() && stm->nIns() == 3 &&
    dynamic_cast<ConstantNode*>(off()) && hoistPtr(ptr1, memphi)) {
        if(profit(stm, 2) || (!dynamic_cast<LoopNode*>(stm->region()) && profit(stm, 1))) {
                Node *ld1 = ld(1);
                Node *ld2 = ld(2);
                return new PhiNode(name_, declaredType, std::initializer_list < Node * > {stm->region(), ld1, ld2});
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
bool LoadNode::hoistPtr(Node *ptr, PhiNode *memphi) {
    // Can I hoist ptr above the Region?
    auto*r = dynamic_cast<RegionNode*>(memphi->region());
    if(!r) return false;// Dead or dying Region/Phi
    // If ptr from same Region, then yes, just use hoisted split pointers
    auto*pphi = dynamic_cast<PhiNode*>(ptr);
    if(pphi && pphi->region() == r) return true;

    // No, so can we lift this ptr?
    CFGNode*cptr=  ptr->cfg0();
    if(cptr != nullptr) {
        // Pointer is controlled high
        // TODO: Really needs to be the LCA of all inputs is high
        return cptr->idepth() <= r->idepth();
    }

    return false;
}
Node* LoadNode::castR0(Node *rez) {
    if(ptr()->type_->isFinal() && !rez->type_->isFinal()) {
        return alloc.new_object<ReadOnlyNode>(rez)->peephole();
    }
    return rez;

}
bool LoadNode::profit(PhiNode* phi, int idx) {
    Node* px = phi->in(idx);
    if(px == nullptr) return false;
    auto*mem = dynamic_cast<TypeMem*>(px->type_);
    auto*st1 = dynamic_cast<StoreNode*>(px);
    if(mem && mem->t_->isHighOrConst()) {
        px->addDep(this);
        return true;
    }
    if(st1 && ptr() == st1->ptr() && off() == st1->off()){
        px->addDep(this);
        return true;
    }
    return false;
}

Node* LoadNode::ld(int idx) {
    Node* mem1 = mem();
    Node*ptr1 = ptr();
    bool pt = dynamic_cast<PhiNode*>(ptr1) && ptr1->in(0) == mem1->in(0);
    return alloc.new_object<LoadNode>(name_, alias_, declaredType, mem1->in(idx), pt ? ptr1->in(idx) : ptr1, off())->peephole();
}

bool LoadNode::neverAlias(Node *ptr1, Node *ptr2) {
    return ptr1->in(0) != ptr2->in(0) && dynamic_cast<ProjNode*>(ptr1) && dynamic_cast<NewNode*>(ptr1->in(0)) &&
    dynamic_cast<ProjNode*>(ptr2) && dynamic_cast<NewNode*>(ptr2->in(0));
}
//Node* LoadNode::copy(Node *mem_, Node *ptr) {
//    return alloc.new_object<LoadNode>(name_, alias_, declaredType, mem_, ptr);
//}