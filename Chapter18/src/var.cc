#include "../Include/var.h"

Var::Var(int idx, std::string name, Type* type, bool xfinal, Lexer loc) : name_(name), idx_(idx), type_(type), final_(xfinal), loc_(loc), fref_(false) {}

Var::Var(int idx, std::string name, Type* type, bool xfinal, Lexer loc, bool fref) : name_(name), idx_(idx), type_(type), final_(xfinal), loc_(loc), fref_(fref) {}

Type *Var::type() {
    if(!type_->isFRef()) return type_;
    // Update self to no longer use the forward ref type
    Type*def = Parser::TYPES.get(dynamic_cast<TypeMemPtr*>(type_)->obj_->name_));
    type_ = type_->meet(def);
    return type_;
}
Type* Var::lazyGLB() {
    Type*t = type();
    if(dynamic_cast<TypeMemPtr*>(t)) {
        return t;
    }
    return t->glb();
}

bool Var::isFref() {
    return fref_;
}

void Var::defFRef(Type *type, bool xfinal, Lexer loc) {
    assert(fref_);
    assert(type_ == Type::BOTTOM);
    type_ = type;
    final_ = xfinal;
    loc_ = loc;
    fref_ = false;
}

std::string Var::ToString() {
    return type_->ToString() + (final_ ? " " : " !") + name_;
}