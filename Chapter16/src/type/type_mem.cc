#include "../../Include/type/type_mem.h"
#include "../../Include/type/integer_type.h"

TypeMem::TypeMem(int alias, Type*t) : Type(TMEM), alias_(alias), t_(t) {}

TypeMem *TypeMem::make(int alias, Type*t) {
    return dynamic_cast<TypeMem *>((alloc.new_object<TypeMem>(alias, t))->intern());
}

TypeMem *TypeMem::TOP() {
    static TypeMem *top = make(0, Type::TOP());
    return top;
}

TypeMem *TypeMem::BOT() {
    static TypeMem *bot = make(-1, Type::BOTTOM());
    return bot;
}

void TypeMem::gather(Tomi::Vector<Type *> &ts) {
    ts.push_back(make(1, TypeInteger::ZERO()));
    ts.push_back(BOT());
}

TypeMem *TypeMem::xmeet(Type *t) {
    auto *that = dynamic_cast<TypeMem *>(t);
    if(this == TOP()) return that;
    if(that == TOP()) return this;

    if(this == BOT()) return BOT();
    if(that == BOT()) return BOT();
    int alias = alias_ == that->alias_ ? alias_ : -1;
    // how is that we need to introduce a type suddnely here
    Type* mt = t_->meet(that->t_);
    return make(alias, mt);
}

Type *TypeMem::dual() {
    if (alias_ == 0) return BOT();
    if (alias_ == -1) return TOP();
    return make(alias_, t_->dual());
}

Type *TypeMem::glb() {
    return make(alias_, t_->glb());
}

int TypeMem::hash() {
    return 9876543 + alias_ + t_->hash();
}

bool TypeMem::eq(Type *t) {
    auto *that = dynamic_cast<TypeMem *>(t);
    return alias_ == that->alias_ && t_ == that->t_;
}

std::ostringstream &TypeMem::print_1(std::ostringstream &builder) {
    builder << "MEM#";
    switch (alias_) {
        case 0:
            builder << "TOP";
            break;
        case -1:
            builder << "BOT";
            break;
        default:
            builder << std::to_string(alias_);
            builder << ":";
            t_->print_1(builder);
            break;
    }
    return builder;
}

std::string TypeMem::str() {
    std::ostringstream builder;
    return print_1(builder).str();
}