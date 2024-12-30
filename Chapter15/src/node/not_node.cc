#include "../../Include/node/not_node.h"
#include "../../Include/type/integer_type.h"
#include "../../Include/type/type_mem_ptr.h"
#include "../../Include/type/type_float.h"

NotNode::NotNode(Node *in) : Node({nullptr, in}) {
}

std::string NotNode::label() { return "Not"; }

std::string NotNode::glabel() { return "!"; }

std::ostringstream &NotNode::print_1(std::ostringstream &builder, Tomi::Vector<bool>& visited) {
    builder << "(!";
     in(1)->print_1(builder, visited);
    builder << ")";
    return builder;
}

Type *NotNode::compute() {
    if(in(1)->type_->isHigh()) return TypeInteger::BOOL()->dual();
    if (auto *i = dynamic_cast<TypeInteger *>(in(1)->type_)) {
        return i->max_ < 0 || i->min_ > 0? TypeInteger::FALSE() : (i == TypeInteger::ZERO() ? TypeInteger::TRUE() : TypeInteger::BOT());
    }
    if (auto* i = dynamic_cast<TypeFloat*>(in(1)->type_)) {
        if (i->isConstant()) {
            return TypeInteger::constant(i->value() == 0 ? 1 : 0);
        } else {
            return i;
        }
    }
    if(auto* p0 = dynamic_cast<TypeMemPtr*>(in(1)->type_)) {
        // top->top, bot->bot, null->1, *void->0, not-null ptr->0, ptr/nil->bot
        // If input in null then true
        // If input is not null ptr then false
        if(p0 == TypeMemPtr::TOP()) return TypeInteger::TOP();
        if(p0 == TypeMemPtr::NULLPTR()) return TypeInteger::constant(1);
        if(!p0->nil_) return TypeInteger::constant(0);
        return TypeInteger::BOT();
    }
    if(dynamic_cast<Type*>(in(1)->type_)) {
        auto* t = in(1)->type_;
        if(typeid(*t) != typeid(Type)) {
            // other subclasses are not considered
            throw std::runtime_error("Only doing it on ints and ptrs");
        }
        return t == Type::TOP() ? Type::TOP() : Type::BOTTOM();
    }
    return TypeInteger::TOP()->meet(in(1)->type_);
}

Node *NotNode::idealize() { return nullptr; }
