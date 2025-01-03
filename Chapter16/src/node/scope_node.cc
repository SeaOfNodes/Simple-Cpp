#include "../../Include/node/scope_node.h"
#include "../../Include/node/cast_node.h"
#include "../../Include/node/constant_node.h"
#include "../../Include/parser.h"

#include <cassert>

ScopeNode::ScopeNode() : ScopeMinNode({}) { type_ = Type::BOTTOM(); }

std::string ScopeNode::label() { return "Scope"; }

Type *ScopeNode::compute() { return Type::BOTTOM(); }

Node *ScopeNode::idealize() { return nullptr; }

void ScopeNode::push() {
    lexSize.emplace_back();
    //decls.emplace_back();
}

void ScopeNode::pop() {
    lexSize.pop_back();
    int n = lexSize.back();
    popUntil(n);
    vars.resize(n);

//    // first pop elements in hashmap
//    popN(idxs.back().size());
//    // then pop the empty hashmap
//    idxs.pop_back();
//    decls.pop_back();
}

Node *ScopeNode::upcast(Node *ctrl, Node *pred, bool invert) {
    if (ctrl->type_ == Type::XCONTROL()) return nullptr;
    // Invert the If conditional
    if (invert) {
        auto *notNode = dynamic_cast<NotNode *>(pred);
        if (notNode) {
            pred = notNode->in(1);
        } else {
            pred = IterPeeps::add((new NotNode(pred))->peephole());
        }
    }
    // Direct use of a value as predicate.  This is a zero/null test.
    auto *it = std::find(inputs.begin(), inputs.end(), pred);
    if (it != inputs.end()) {
        auto *tmp = dynamic_cast<TypeMemPtr *>(pred->type_);
        if (!tmp) {
            // Must be an `int`, since int and ptr are the only two value types
            // being tested. No representation for a generic not-null int, so no upcast.
            return nullptr;
        }
        if (tmp->isa(TypeMemPtr::VOIDPTR())) {
            return nullptr;  // Already not-null, no reason to upcast

        }
        // Upcast the ptr to not-null ptr, and replace in scope
        return replace(pred, (alloc.new_object<CastNode>(TypeMemPtr::VOIDPTR(), ctrl, pred))->peephole());

    }
    if(auto* NOT = dynamic_cast<NotNode*>(pred)) {
        // Direct use of a !value as predicate.  This is a zero/null test.
        auto* it = std::find(inputs.begin(), inputs.end(), NOT->in(1));
        if(it != inputs.end()) {
            Type*tinit = NOT->in(1)->type_->makeInit();
            if(NOT->in(1)->type_->isa(tinit)) return nullptr;
            return replace(NOT->in(1), (alloc.new_object<ConstantNode>(tinit, Parser::START))->peephole());
        }
    }
    return nullptr;
}

int ScopeNode::find(std::string name) {
    for(int i = vars.size() -1; i >= 0; i--) {
        if(vars[i]->name_ == name) return i;
    }
    return -1;
}
Node *ScopeNode::replace(Node *old, Node *cast) {
    for (int i = 0; i < nIns(); i++) {
        if (in(i) == old) {
            setDef(i, cast);
        }
    }
    return cast;
}

// add it here
bool ScopeNode::define(std::string name, Type *declaredType, bool xfinal, Node *init) {
    if (lexSize.size() > 1) {
        for(int i  = vars.size()-1; i >= lexSize.back(); i-- ) {
            if(vars[i]->name_ == name) return false;
        }
    }
    vars.push_back(alloc.new_object<ScopeMinNode::Var>(nIns(), name, declaredType, xfinal));
    addDef(init);
    return true;
}

Node* ScopeNode::mem(int alias) {
    return mem()->mem_(alias, nullptr);
}
void ScopeNode::mem(int alias, Node*st) {
    mem()->mem_(alias, st);
}
ScopeMinNode::Var *ScopeNode::lookup(std::string name) {
    int idx = find(name);
    return idx == -1 ? nullptr : update_(vars[idx], nullptr);
}

void ScopeNode::update(std::string name, Node *n) {
    int idx = find(name);
    update_(vars[idx], n);
}

ScopeMinNode::Var *ScopeNode::update_(ScopeMinNode::Var* v, Node *st) {
//    // nesting level is negative if nothing is found
//    if (nestingLevel < 0) // Missed in all scopes, not found
//        return nullptr;
//
//    auto syms = idxs[nestingLevel]; // Get the symbol table for nesting level
//    auto idx = syms.get(name);
//    // Not found in this scope, recursively look in parent scope
//    if (idx == nullptr)
//        return update(name, n, nestingLevel - 1);
    Node *old = in(v->idx_);
    if (auto *loop = dynamic_cast<ScopeNode *>(old)) {
        // Lazy Phi!
        Node*def = loop->in(v->idx_);
        auto *phi =
                dynamic_cast<PhiNode *>(def);
        if (phi && loop->ctrl() == phi->region()) {
            old = def;
        } else {
            Node* new_node =(alloc.new_object<PhiNode>(v->name_, dynamic_cast<TypeMemPtr*>(v->type_) ?
                                                                                  v->type_ : v->type_->glb(),
                                                       std::initializer_list<Node *>{loop->ctrl(),
                                                                                     loop->in(loop->update_(v, nullptr)->idx_),
                                                                                     nullptr}))->peephole();

            old = loop->setDef(
                    v->idx_, new_node);
        }
        setDef(v->idx_, old);
    }
    // If n is null we are looking up rather than updating, hence return existing
    // value
    if(st != nullptr) setDef(v->idx_, st);
    return v;
}

Node *ScopeNode::ctrl() { return in(0); }

Node *ScopeNode::ctrl(Node *n) { return setDef(0, n); }

ScopeMinNode* ScopeNode::mem() {
    return dynamic_cast<ScopeMinNode *>(in(1));
}
std::ostringstream &ScopeNode::print_1(std::ostringstream &builder,
                                       Tomi::Vector<bool> &visited) {
    builder << "Scope [ ";
    int j = 1;

    for(int i = 0; i < nIns(); i++) {
        if(j < lexSize.size() && i == lexSize[j]) {
            builder << " | ";
            j++;
        }
        ScopeMinNode::Var* v = vars[i];
        v->type_->print_1(builder);
        builder << " ";
        if(v->final_) builder << "!";
        builder << v->name_;
        builder << "=";
        Node*n = in(i);

        while(auto* loop = dynamic_cast<ScopeNode*>(n)) {
            builder << "Lazy_";
            n = loop->in(i);
        }
        if(n == nullptr) builder << "___";
        else n->print_0(builder, visited);
        builder << ", ";
    }
    builder << "]";
    return builder;
//    builder << label();
//    builder << "[";
//    keys.reserve(idxs.size());
//    Tomi::Vector<std::string> names = reverseNames();
//    for (int j = 0; j < nIns(); j++) {
//        builder << names[j] << ":";
//        Node *n = in(j);
//        while (auto *loop = dynamic_cast<ScopeNode *>(n)) {
//            builder << "Lazy_";
//            n = loop->in(j);
//        }
//        n->print_0(builder, visited);
//    }
//    builder << "]";
//
//    return builder;
}

Node *ScopeNode::mergeScopes(ScopeNode *that) {
    // not called with keep here
    RegionNode *r = dynamic_cast<RegionNode *>(
            ctrl((alloc.new_object<RegionNode>(
                    std::initializer_list<Node *>{nullptr, ctrl(), that->ctrl()}))->keep()));

    mem()->merge_(that->mem(), r);
    merge_(that, r);
    // Tomi::Vector<std::string> ns = reverseNames();
    // Note that we skip i==0, which is bound to '$ctrl'
//    for (int i = 1; i < nIns(); i++) {
//        if (in(i) != that->in(i)) { // No need for redundant Phis
//            // If we are in lazy phi mode we need to a lookup
//            // by name as it will trigger a phi creation
////            if(ns[i] == "p") {
////                std::cout << "here";
////            }
////            if(ns[i] == "h") {
////                std::cout << "here";
////            }
//            Node *phi =
//                    alloc.new_object<PhiNode>(ns[i], lookUpDeclaredType(ns[i]), std::initializer_list<Node *>
//                            {r, this->lookup(ns[i]), that->lookup(ns[i])});
//            phi = phi->peephole();
//            setDef(i, phi);
//        }
//    }
    that->kill();
    IterPeeps::add(r);
    return r->unkeep()->peephole();
}

void ScopeNode::merge_(ScopeNode*that, RegionNode*r) {
    for(int i = 0; i < nIns(); i++) {
        if(in(i) != that->in(i)) {
            // No need for redundant Phis
            // If we are in lazy phi mode we need to a lookup
            // by name as it will trigger a phi creation
            ScopeMinNode::Var*v = vars[i];
            Node*lhs = in(update_(v, nullptr));
            Node*rhs = that->in(update_(v, nullptr));
            setDef(i, alloc.new_object<PhiNode>(v->name_, v->type_, std::initializer_list<Node*>{r, lhs, rhs})->peephole());
        }
    }
}
void ScopeNode::endLoop(ScopeNode *back, ScopeNode *exit) {
    Node *ctrl1 = ctrl();
    auto *loop = dynamic_cast<LoopNode *>(ctrl1);
    assert(loop && loop->inProgress());
    ctrl1->setDef(2, back->ctrl());
    mem()->endLoopMem_(this, back->mem(), exit->mem());
    endLoopMem_(this, back, exit);
    back->kill();

    mem()->useless_();
    this->useless_();

    // The exit mem's lazy default value had been the loop top,
    // now it goes back to predating the loop.

    exit->mem()->setDef(1, mem()->in(1));
}

void ScopeNode::endLoopMem_(ScopeNode* scope, Node*back, Node*exit) {
    for(int i = 2; i < nIns(); i++) {
        if(back->in(i) != scope) {
            PhiNode*phi = dynamic_cast<PhiNode*>(in(i));
            phi->setDef(2, back->in(i));
        }
        if(exit->in(i) == scope) {
            exit->setDef(i, in(i));
        }
    }
}
ScopeNode *ScopeNode::dup() { return dup(false); }

//Type *ScopeNode::lookUpDeclaredType(std::string name) {
//    for (size_t i = decls.size(); i > 0; i--) {
//        Type **t = decls[i - 1].get(name);
//        if (t != nullptr) return *t;
//    }
//    return nullptr;
//}

ScopeNode *ScopeNode::dup(bool loop) {
    auto *dup = alloc.new_object<ScopeNode>();
    // Our goals are:
    // 1) duplicate the name bindings of the ScopeNode across all stack levels
    // 2) Make the new ScopeNode a user of all the nodes bound
    // 3) Ensure that the order of defs is the same to allow easy merging
    for (const auto &syms: vars) {
        dup->vars.push_back(syms);
    }
    for (const auto &declaredType: lexSize) {
        dup->lexSize.push_back(declaredType);
    }
    // Control comes first
    dup->addDef(ctrl());

    // Memory input is a shallow copy
    ScopeMinNode* memdup = alloc.new_object<ScopeMinNode>();
    ScopeMinNode* mem1 = mem();
    memdup->addDef(nullptr);
    memdup->addDef(loop ? this : mem1->in(1));

    for(int i = 2; i < mem1->nIns(); i++) {
        memdup->addDef(loop ? this : mem1->in(i));
    }
    dup->addDef(memdup);
    // now, all the inputs
    for (int i = 2; i < mem()->nIns(); i++) {
        // For lazy phis on loops we use a sentinel
        // that will trigger phi creation on update
        memdup->addDef(loop ? this : mem1->in(i));
    }
    return dup;
}

//Tomi::Vector<std::string> ScopeNode::reverseNames() {
//    Tomi::Vector<std::string> names(nIns());
//    for (auto &syms: idxs) {
//        for (auto pair: syms) {
//            names[pair.val] = pair.key;
//        }
//    }
//    return names;
//}

std::string ScopeNode::CTRL = "$ctrl";
std::string ScopeNode::ARG0 = "arg";
std::string ScopeNode::MEM0 = "$mem";