#include "../../Include/type/type_float.h"
#include <iostream>
#include <iomanip>
#include <cmath>
#include <cstdint>

TypeFloat *TypeFloat::TOP() {
    static TypeFloat *top = make(static_cast<std::int8_t>(-64), 0);
    return top;
}

TypeFloat *TypeFloat::BOT() {
    static TypeFloat *bot =  make(static_cast<std::int8_t>(-32), 0);
    return bot;
}

TypeFloat *TypeFloat::ZERO() {
    static TypeFloat *zero =  make(static_cast<std::int8_t>(0), 0);
    return zero;
}

TypeFloat *TypeFloat::T32() {
    static TypeFloat *t32 =  make(static_cast<std::int8_t>(32), 0);
    return t32;
}

TypeFloat* TypeFloat::B32() {
    static TypeFloat *b32 =  make(static_cast<std::int8_t>(64), 0);
    return b32;
}

TypeFloat::TypeFloat(std::int8_t sz, double con) : Type(TFLT), sz_(sz), con_(con) {
}

TypeFloat *TypeFloat::make(std::int8_t sz, double con) {
    return dynamic_cast<TypeFloat*>((alloc.new_object<TypeFloat>(sz, con))->intern());
}

TypeFloat *TypeFloat::constant(double con) {
    return make(static_cast<std::int8_t>(0), con);
}

void TypeFloat::gather(Tomi::Vector<Type *> &ts) {
    ts.push_back(ZERO());
    ts.push_back(BOT());
    ts.push_back(B32());
    ts.push_back(constant(3.141592653589793));
}

std::ostringstream &TypeFloat::print_1(std::ostringstream &builder) {
    if (this == TOP())
        builder << "FloatTop";
    if (this == BOT())
        builder << "FloatBot";
    if(this == T32()) {
        builder << "F32Top";
    }
    if(this == B32()) {
        builder << "F32Bot";
    }
    if(std::floor(con_) == con_) {
        builder << std::fixed << std::setprecision(1) << con_;
    } else {
        builder << con_;
    }
    return builder;
}

std::string TypeFloat::str() {
    if(this == TOP()) return "~flt";
    if(this == T32()) return "~f32";
    if(this == B32()) return "f32";
    if(this == BOT()) return "_flt";
    return std::to_string(con_) + (isF32() ? "f" : "");
}

std::ostringstream &TypeFloat::typeName(std::ostringstream &builder) {
    if(this == TOP()) builder << "FltTop";
    if(this == T32()) builder << "F32Top";
    if(this == B32()) builder << "F32Bot";
    if(this == BOT()) builder << "FltBot";
    if(isF32()) {
        builder << "F32";
    } else {
        builder << "Flt";
    }
    return builder;
}

bool TypeFloat::isF32() {
    return static_cast<float>(con_) == con_;
}
bool TypeFloat::isHigh() {
    return sz_ < 0;
}

bool TypeFloat::isHighOrConst() {
    return sz_ <=0;
}

bool TypeFloat::isConstant() {
    return sz_ == 0;
}

double TypeFloat::value() {
    return con_;
}

Type *TypeFloat::xmeet(Type *other) {
    TypeFloat* i = dynamic_cast<TypeFloat*>(other);
    TypeFloat* i0  = sz_ < i->sz_ ? this : i;
    TypeFloat* i1 = sz_ < i->sz_ ? i : this;

    if(i1->sz_ == 64) return BOT();
    if(i0->sz_ == -64) return i1;

    if(i1->sz_ == 32) {
        return i0->sz_ == 0 && !i0->isF32() ? BOT() : B32();
    }
    if(i1->sz_ != 0) return i1;

    // i1 is a constant

    if(i0->sz_ == -32) {
        return i1->isF32() ? i1 : BOT();
    }

    return i0->isF32() && i1->isF32() ? B32() : BOT();
}

Type *TypeFloat::dual() {
    if (sz_ == 0) return this;
    return make(static_cast<std::int8_t>(-sz_), con_);
}

Type *TypeFloat::glb() {
    return BOT();
}

TypeFloat *TypeFloat::makeInit() {
    return ZERO();
}

int TypeFloat::hash() {
    return static_cast<int>(con_) ^ (sz_);
}

bool TypeFloat::eq(Type *other) {
    auto *t = dynamic_cast<TypeFloat *>(other);
    return (con_ == t->con_) && (sz_ == t->sz_);
}
unsigned long long Tomi::hash<TypeFloat*>::operator()(TypeFloat* val) {
    return val->hashCode();
}