#include "../../Include/type/type_mem.h"

TypeMem::TypeMem(int alias) : Type(TMEM), alias_(alias) {}

TypeMem *TypeMem::make(int alias) {
    return static_cast<TypeMem *>((new TypeMem(alias))->intern());
}

TypeMem *TypeMem::TOP() {
    static TypeMem *top = make(0);
    return top;
}

TypeMem *TypeMem::BOT() {
    static TypeMem *bot = make(-1);
    return bot;
}

void TypeMem::gather(Tomi::Vector<Type *> &ts) {
    ts.push_back(make(1));
    ts.push_back(BOT());
}

TypeMem *TypeMem::xmeet(Type *t) {
    TypeMem *that = dynamic_cast<TypeMem *>(t);
    return alias_ == 0 ? that : (that->alias_ == 0 ? this : BOT());
}

Type *TypeMem::dual() {
    if (alias_ == 0) return BOT();
    if (alias_ == -1) return TOP();
    return this;
}

Type *TypeMem::glb() {
    return TypeMem::BOT();
}

int TypeMem::hash() {
    return 9876543 + alias_;
}

bool TypeMem::eq(Type *t) {
    TypeMem *that = dynamic_cast<TypeMem *>(t);
    return alias_ == that->alias_;
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
            break;
    }
    return builder;
}

std::string TypeMem::str() {
    std::ostringstream builder;
    return print_1(builder).str();
}