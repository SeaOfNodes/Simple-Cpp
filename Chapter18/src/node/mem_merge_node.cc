#include "../../Include/node/mem_merge_node.h"
#include "../../Include/type/type_mem.h"

MergeMemNode::MergeMemNode(bool inProgress) : type_(TypeMem::BOT()), inProgress_(inProgress){}
bool MergeMemNode::inProgress() { return inProgress_; }

std::string MergeMemNode::label() {
    return "ALLMEM";
}
bool MergeMemNode::isMem() {
    return true;
}

std::ostringstream& MergeMemNode::print_1(std::ostringstream &builder, Tomi::Vector<bool> &visited) {
    builder << (inProgress_ ? "Merge[ ": "MEM[ ");
    for(int j = 2; j<nIns(); j++) {
        builder << j;
        builder << ":";
        Node*n = in(j);
        while(auto*loop = dynamic_cast<ScopeNode*>(n)) {
            builder << "Lazy_";
            n = loop->mem(j);
        }
        if(n == nullptr) builder << "___ ";
        else {
            n->print_0(builder, visited);
            builder << " ";
        }
    }
    builder << "]";
    return builder;
}

Node *MergeMemNode::merge() {
    // Force default memory to not be lazy
    MergeMemNode*merge = alloc.new_object<MergeMemNode>(false);
    for(Node*n: inputs) {
        merge->addDef(n);
    }
    merge->mem_(1, nullptr);
    return merge->peephole();
}

Type* MergeMemNode::compute() {
    return TypeMem::BOT();
}
Node* MergeMemNode::idealize() {
   if(inProgress()) return nullptr;

    // If not merging any memory (all memory is just the default)
    if(nIns() == 2) return in(1);

    return nullptr;
}

Node* MergeMemNode::in(ScopeMinNode::Var *v) {
    return in(v->idx());
}
Node* MergeMemNode::alias(int alias) {
    return in(alias<nIns() && in(alias) != nullptr ? alias : 1);
}
Node*MergeMemNode::alias(int alias, Node*st) {
    while(alias >= nIns()) addDef(nullptr);
    return setDef(alias, st);
}
Node*MergeMemNode::mem_(int alias, Node*st) {
    // Memory projections are made lazily; if one does not exist
    // then it must be START.proj(1)
    Node*old = alias(alias);
    auto*loop = dynamic_cast<ScopeNode*>(old);
    if(loop) {
        MergeMemNode* loopmem = loop->mem();
        Node*memdef = loopmem->alias(alias);
        // Lazy phi!
        auto*phi = dynamic_cast<PhiNode*>(memdef);
        if(phi && loop->ctrl() == phi->region()) {
            old = memdef;
        } else {
            old = loopmem->alias(alias, alloc.new_object<PhiNode>(Parser::memName(alias), TypeMem::BOT(), loop->ctrl(), loopmem->mem_(alias,nullptr), nullptr)->peephole());

        }
    }
    // Memory projections are made lazily; expand as needed
    return st == nullptr ? old :
}
void MergeMemNode::merge_(MemMergeNode*that, RegionNode*r){
    int len = std::max(nIns(), that->nIns());
    for(int i =2; i < len; i++) {
        if(alias(i) != that->alias(i)) {
            // If we are in lazy phi mode we need to a lookup
            // by alias as it will trigger a phi creation
            Node*lhs = mem_(i, nullptr);
            Node*rhs = that->mem_(i, nullptr);
            alias(i, alloc.new_object<PhiNode>(Parser::memName(i), lhs->type_->glb()->meet(rhs->type_->glb()), r, lhs, rhs)->peephole());
        }
    }
}

void MergeMemNode::endLoopMem_(ScopeNode *scope, MergeMemNode *back, int *exit) {
    Node*exit_def = exit->alias(1);
    for(int i = 1; i < nIns(); i++) {
        auto*phi = dynamic_cast<PhiNode*>(in(i));
        if(phi && phi->region() == scope->ctrl()) {
            phi->setDef(2, back->in(i) == scope ? phi: back->in(i));
        }
        if(exit_def == scope) {
            exit->alias(i, in(i));
        }
    }
}

void MergeMemNode::useless_() {
    for(int i = 2; i < nIns(); i++) {
        auto*phi = dynamic_cast<PhiNode*>(in(i));
        if(phi) {
            Node*phi = phi->peephole();
            IterPeeps::addAll(phi->outputs);
            phi->moveDepsToWorkList();
            if(in != phi) {
                if(!phi->iskeep()) phi->subsume(in);
            setDef(i, in);
            }
        }
    }
}

bool MergeMemNode::eq(Node *n) {
    return this == n || !inProgress_;
}