#include "../../Include/node/store_node.h"
#include "../../Include/type/type_mem.h"
#include "../../Include/type/type_mem_ptr.h"
#include "../../Include/node/proj_node.h"
#include "../../Include/node/new_node.h"
#include "../../Include/parser.h"

StoreNode::StoreNode(Lexer *loc_, std::string name, int alias, Type *glb, Node *mem, Node *ptr, Node *off, Node *value,
                     bool init_) : MemOpNode(loc_, name, alias,
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
    Type *val_ = val()->type_;
    auto *mem1 = dynamic_cast<TypeMem *>(mem()->type_);
    if (mem1 == TypeMem::TOP()) return TypeMem::TOP();
    Type *t = Type::BOTTOM();      // No idea on field contents
    // Same alias, lift val to the declared type and then meet into other fields
    if (mem1->alias_ == alias_) {
        auto *tmp = dynamic_cast<TypeMemPtr *>(val_);
        // Update declared forward ref to the actual
        if (declaredType->isFRef() && tmp && !tmp->isFRef()) {
            declaredType = tmp;
        }
        val_ = val_->join(declaredType);
        t = val_->meet(mem1->t_);
    }

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
        st->checkOnlyUse(st)) {
        setDef(1, st->mem());
        return this;
    }
    // Simple store-after-new on same address.  Should pick up the
    // an init-store being stomped by a first user store.
    if (auto st = dynamic_cast<ProjNode *>(mem())) {
        if (auto nnn = dynamic_cast<NewNode *>(st->in(0))) {
            if (auto ptrProj = dynamic_cast<ProjNode *>(ptr())) {
                if (ptrProj->in(0) == nnn) {
                    if (auto tmp = dynamic_cast<TypeMemPtr *>(ptr()->type_)) {
                        if (!(tmp->obj_->isAry() && tmp->obj_->fields_.value()[1]->alias_ == alias_) &&
                            val()->type_->isHighOrConst() &&
                            checkOnlyUse(st)) {
                            nnn->setDef(nnn->find_alias(alias_), val());
                            nnn->type_ = nnn->compute();
                            mem()->type_ = mem()->compute();
                            // Must *retype* the NewNode, this is not monotonic in isolation
                            // but is monotonic counting from this Store to the New.
                            return mem();
                        }
                    }
                }
            }
        }
    }

    return nullptr;
}

// Check that `this` has no uses beyond `that`
bool StoreNode::checkOnlyUse(Node *mem) {
    if (mem->nOuts() == 1) return true;
    // Add deps on the other uses (can be e.g. ScopeNode mid-parse) so that
    // when the other uses go away we can retry.
    for (Node *use: mem->outputs) {
        if (use != this) {
            use->addDep(this);
        }
    }
    return false;
}

// Todo: Parser.parseException
std::string StoreNode::err() {
    std::string err = MemOpNode::err();
    if (!err.empty()) return err;
    TypeMemPtr *tmp = dynamic_cast<TypeMemPtr *>(ptr()->type_);
    if (tmp->obj_->field(name_)->final_) {
        return "Cannot modify final field '" + name_ + "'";
    }
    Type *ptr = val()->type_;
    return (init || ptr->isa(declaredType) ? "" : "Cannot store " + ptr->str() + " into field " + declaredType->str() +
                                                  " " + name_);
}