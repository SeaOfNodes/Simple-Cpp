#include "../../Include/node/store_node.h"
#include "../../Include/type/type_mem.h"
#include "../../Include/type/type_mem_ptr.h"

StoreNode::StoreNode(std::string name, int alias, Type*glb, Node*mem, Node*ptr, Node*off, Node* value,  bool init_) : MemOpNode(name, alias,
                                                                                                                     glb, mem, ptr,
                                                                                                         off,
                                                                                                         value), init(init_) {}
std::string StoreNode::label() { return "." + name_ + "="; }

std::string StoreNode::glabel() { return "." + name_ + "="; }

bool StoreNode::isMem() { return true; }

Node *StoreNode::val() { return in(4); }

std::ostringstream &StoreNode::print_1(std::ostringstream &builder, Tomi::Vector<bool> &visited) {
    builder << "." << name_ << " = ";
    val()->print_1(builder, visited);
    builder << ";";
    return builder;
}

Type *StoreNode::compute() {
    Type*val_ = val()->type_;
    auto* mem1 = dynamic_cast<TypeMem*>(mem()->type_);
    Type*t = mem1->alias_ == alias_ ? val_->meet(mem1->t_) : Type::BOTTOM();
    return TypeMem::make(alias_, t);
}

Node *StoreNode::idealize() {
    // Simple store-after-store on same address.  Should pick up the
    // required init-store being stomped by a first user store.
    auto *st = dynamic_cast<StoreNode *>(mem());
    if (st && ptr() == st->ptr() &&
        off() == st->off() && // Must check same object
        dynamic_cast<TypeMemPtr *>(ptr()->type_) &&
        // No bother if weird dead pointers
        // Must have exactly one use of "this" or you get weird
        // non-serializable memory effects in the worse case.
        st->checkNoUseBeyond(this)) {
        setDef(1, st->mem());
        return this;
    }
    return nullptr;
}

// Check that `this` has no uses beyond `that`
bool StoreNode::checkNoUseBeyond(Node *that) {
    if (nOuts() == 1) return true;
    // Add deps on the other uses (can be e.g. ScopeNode mid-parse) so that
    // when the other uses go away we can retry.
    for (Node *use: outputs) {
        if (use != that) {
            use->addDep(that);
        }
    }
    return false;
}

std::string StoreNode::err() {
    std::string err = MemOpNode::err();
    if (!err.empty()) return err;
    Type* ptr = val()->type_;
    return (init || ptr->isa(declaredType) ? "" : "Cannot store " + ptr->str() + " into field " + declaredType->str() + " " + name_);
}