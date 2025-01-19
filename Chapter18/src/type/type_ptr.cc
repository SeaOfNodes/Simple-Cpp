#include "../../Include/type/type_ptr.h"

TypePtr::TypePtr(unsigned int nil) : TypeNil(TPTR, nil) {}


// An abstract pointer, pointing to either a Struct or an Array.
// Can also be null or not, so 4 choices {TOP,BOT} x {nil,not}

TypePtr *TypePtr::XPTR() {
    return alloc.new_object<TypePtr>(0)->intern();
}

TypePtr *TypePtr::XNPTR() {
    return alloc.new_object<TypePtr>(1)->intern();
}

TypePtr *TypePtr::NPTR() {
    return alloc.new_object<TypePtr>(2)->intern();
}
TypePtr *TypePtr::PTR() {
    return alloc.new_object<TypePtr>(3)->intern();
}

void TypePtr::gather(Tomi::Vector<Type *> &ts) {
    ts.push_back(NPTR());
    ts.push_back(PTR());
}

TypeNil *TypePtr::makeFrom(unsigned int nil) {
    throw std::runtime_error("TODO!");
}

Type *TypePtr::xmeet(Type *other) {
    TypePtr*that = dynamic_cast<TypePtr*>(other);
    return PTRS[xmeet0(that)];
}

TypePtr* TypePtr::dual() {
    return PTRS[dual0()];
}

TypeNil *TypePtr::nmeet(TypeNil *tn) {
    if(nil_ == 0) return tn;
    if(nil_ == 1) return tn->makeFrom(xmeet0(tn));
    if(nil_ == 2) return tn->nil_ == 3 ? TypePtr::PTR() ? NPTR();
    return TypePtr::PTR(); // this
}

// RHS is  NIL
Type *TypePtr::meet0() {
    return isHigh() ? NIL() : PTR();
}
// RHS is XNIL
// 0->xscalar, 1->nscalar, 2->nscalar, 3->scalar
Type* TypePtr::meetX() {
    return nil_ == 0 ? XNIL() : (nil_ == 3 ? PTR() : NPTR());
}

TypePtr *TypePtr::glb() {
    return PTR();
}
std::string TypePtr::str() {
    return STRS[nil_];
}

std::ostringstream &TypePtr::print(std::ostringstream &builder) {
    return builder << STRS[nil_];
}