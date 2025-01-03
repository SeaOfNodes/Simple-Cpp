#include "../../Include/node/scope_minnode.h"
#include "../../Include/type/type_mem.h"
#include "../../Include/parser.h"

ScopeMinNode::Var::Var(int idx, std::string name, Type *type, bool final) {
    idx_ = idx;
    name_ = name;
    type_ = type;
    final_ = final;
}

ScopeMinNode::ScopeMinNode() {
    type_ = TypeMem::BOT();
}

std::string ScopeMinNode::label() {
    return "MEM";
}

std::ostringstream &ScopeMinNode::print_1(std::ostringstream &builder, Tomi::Vector<bool> &visited) {
    builder << "MEM[ ";
    for (int j = 2; j < nIns(); j++) {
        builder << j;
        builder << ";";
        Node *n = in(j);

        while (auto *loop = dynamic_cast<ScopeNode *>(n)) {
            builder << "Lazy_";
            n = loop->in(j);
        }
        if (n == nullptr) builder << "___ ";
        else n->print_0(builder, visited);
    }
    builder << "]";
}

Type *ScopeMinNode::compute() {
    return TypeMem::BOT();
}

Node *ScopeMinNode::idealize() {
    return nullptr;
}

Node *ScopeMinNode::in(ScopeMinNode::Var *v) {
    return in(v->idx_);
}

Node *ScopeMinNode::alias(int alias) {
    return in(alias < nIns() && in(alias) != nullptr ? alias : 1);
}

Node *ScopeMinNode::alias(int alias, Node *st) {
    while (alias >= nIns()) addDef(nullptr);
    return setDef(alias, st);
}

Node *ScopeMinNode::mem_(int alias_, Node *st) {
    // Memory projections are made lazily; if one does not exist
    // then it must be START.proj(1)
    Node *old = alias(alias_);
    if (auto *loop = dynamic_cast<ScopeNode *>(old)) {
        ScopeMinNode *loopmem = loop->mem();
        Node *memdef = loopmem->alias(alias_);
        // Lazy phi!

        Node *old;
        auto *phi = dynamic_cast<PhiNode *>(memdef);
        if (phi && loop->ctrl() == phi->region()) {
            old = memdef;
        } else {
            old = loopmem->alias(alias_, alloc.new_object<PhiNode>(Parser::memName(alias_), TypeMem::BOT(), std::initializer_list<Node*>{loop->ctrl(), loopmem->mem_(alias_, nullptr), nullptr})->peephole());
            loopmem->mem_(alias_, nullptr)->peephole();
        }
        alias(alias_, old);
    }
    // Memory projections are made lazily; expand as needed
    return st == nullptr ? old : alias(alias_, st); // Not lazy, so this is the answer
}

void ScopeMinNode::merge_(ScopeMinNode *that, RegionNode *r) {
    int len = std::max(nIns(), that->nIns());
    for (int i = 2; i < len; i++) {
        if (alias(i) != that->alias(i)) { // No need for redundant Phis
            // If we are in lazy phi mode we need to a lookup
            // by name as it will trigger a phi creation
            //Var v = _vars.at(i);

            Node *lhs = mem_(i, nullptr);
            Node *rhs = that->mem_(i, nullptr);
            alias(i, alloc.new_object<PhiNode>(Parser::memName(i), TypeMem::BOT(), std::initializer_list<Node*>{r, lhs, rhs})->peephole());
        }
    }
}

void ScopeMinNode::endLoopMem_(ScopeNode
* scope, ScopeMinNode *back, ScopeMinNode * exit)
{
    for (int i = 2; i<back->nIns(); i++) {
        if (back->in(i)!= scope) {
            PhiNode *phi = dynamic_cast<PhiNode *>(in(i));
            phi->setDef(2, back->
            in(i)
        );  // Fill backedge
    }
    if (exit->alias(i)== scope) {
        exit->alias(i, in(i)
);
    }
}
}

// Now one-time do a useless-phi removal
void ScopeMinNode::useless_() {
    for (int i = 2; i < nIns(); i++) {
        if (auto *phi = dynamic_cast<PhiNode *>(in(i))) {
            // Do an eager useless-phi removal
            Node *in = phi->peephole();
            IterPeeps::addAll(phi->outputs);
            phi->moveDepsToWorkList();
            if (in != phi) {
                if (!phi->iskeep()) { // Keeping phi around for parser elsewhere
                    phi->subsume(in);
                }
                setDef(i, in); // Set the update back into Scope
            }
        }
    }
}