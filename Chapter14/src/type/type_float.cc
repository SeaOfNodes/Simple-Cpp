#include "../../Include/type/type_float.h"
#include <iostream>
#include <iomanip>
#include <cmath>
TypeFloat *TypeFloat::TOP() {
    static TypeFloat *top = make(false, 0);
    return top;
}

TypeFloat *TypeFloat::BOT() {
    static TypeFloat *bot =  make(false, 1);
    return bot;
}

TypeFloat *TypeFloat::ZERO() {
    static TypeFloat *zero =  make(true, 0);
    return zero;
}

TypeFloat::TypeFloat(bool is_con, double con) : Type(TFLT), is_con_(is_con), con_(con) {
}

TypeFloat *TypeFloat::make(bool is_con, double con) {
    return dynamic_cast<TypeFloat*>((alloc.new_object<TypeFloat>(is_con, con))->intern());
}

TypeFloat *TypeFloat::constant(double con) {
    return make(true, con);
}

void TypeFloat::gather(Tomi::Vector<Type *> &ts) {
    ts.push_back(ZERO());
    ts.push_back(BOT());
}

std::ostringstream &TypeFloat::print_1(std::ostringstream &builder) {
    if (this == TOP())
        builder << "FloatTop";
    if (this == BOT())
        builder << "FloatBot";
    if(std::floor(con_) == con_) {
        builder << std::fixed << std::setprecision(1) << con_;
    } else {
        builder << con_;
    }
    return builder;
}

std::string TypeFloat::str() {
    if(this == TOP()) return "~flt";
    if(this == BOT()) return "_flt";
    return std::to_string(con_);
}

std::ostringstream &TypeFloat::typeName(std::ostringstream &builder) {
    if(this == TOP()) builder << "FltTop";
    if(this == BOT()) builder << "FltBot";
    return builder;
}

bool TypeFloat::isHighOrConst() {
    return is_con_ || con_ == 0;
}

bool TypeFloat::isConstant() {
    return is_con_;
}

double TypeFloat::value() {
    return con_;
}

Type *TypeFloat::xmeet(Type *other) {
    if (this == other) return this;
    if (other == BOT()) return BOT();
    if (other == TOP()) return this;
    return BOT();
}

Type *TypeFloat::dual() {
    if (isConstant()) return this;
    return con_ == 0 ? BOT() : TOP();
}

Type *TypeFloat::glb() {
    return BOT();
}

TypeFloat *TypeFloat::makeInit() {
    return ZERO();
}

int TypeFloat::hash() {
    return static_cast<int>(con_) ^ (is_con_ ? 0: 0x4000);
}

bool TypeFloat::eq(Type *other) {
    auto *t = dynamic_cast<TypeFloat *>(other);
    return (con_ == t->con_) && (is_con_ == t->is_con_);
}
unsigned long long Tomi::hash<TypeFloat*>::operator()(TypeFloat* val) {
    return val->hashCode();
}