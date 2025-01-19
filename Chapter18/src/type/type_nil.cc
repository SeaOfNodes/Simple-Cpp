#include "../../Include/type/type_nil.h"
#include <limits>
// TOdo: continue from here
TypeNil::TypeNil(unsigned int,t unsigned int nil): Type(t), nil_(nil) {

}

static void TypeNil::gather(Tomi::Vector<Type*>& ts) {

}

unsigned int TypeNil::xmeet0(TypeNil* that) {
    return std::max<unsigned int>(nil_, that->nil_);
}

unsigned int TypeNil::dual0() {
    return static_cast<long>(3-nil_);
}

Type *TypeNil::meetX(Type *other) {
    return nil_==0 ? XNIL() : (nil_<=2 ? TypePtr::NPTR() : TypePtr::PTR());
}

Type* TypeNil::nmeet(TypeNil* tn) {
    // Invariants: both are TypeNil subclasses and unequal classes.
    // If this is TypePtr, we went to TypePtr.nmeet and not here.
    // If that is TypePtr, this is not (invariant); reverse and go again.
    if(auto*ts = dynamic_cast<TypePtr*>(tn)) {
        return ts->nmeet(this)
    }
    // Two mismatched TypeNil, no Scalar.
    if(nil_ == 0 && tn->nil_ == 0) {
        return XNIL();
    }
    if(nil_ <= 2 && tn->nil_ <= 2) {
        return TypePtr::NPTR();
    }
    return TypePtr::PTR();
}

bool TypeNil::isHigh() {
    return nil_ <= 1;
}
bool TypeNil::isConstant() {
    return false;
}
bool TypeNil::isHighOrConstant() {
    return isHigh() || isConstant();
}

Type* TypeNil::glb() {
    return Type::NIL();
}
bool TypeNil::notNull() {
    return nil_ == 1 || nil_ == 2;
}
bool TypeNil::nullable() {
    return nil_ == 3;
}

std::string TypeNil::q() {
    return nil_ == 1 || nil_ == 2 ? "" : "?";
}

std::string TypeNil::x() {
    return isHigh() ? "~" : "";
}

int TypeNil::hash() {
    return nil_ << 17;
}
bool TypeNil::eq(TypeNil *ptr) {
    return nil_ == ptr->nil_;
}
