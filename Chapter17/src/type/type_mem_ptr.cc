#include "../../Include/type/type_mem_ptr.h"
#include <cassert>

TypeMemPtr::TypeMemPtr(TypeStruct *obj, bool nil) : Type(TMEMPTR), obj_(obj), nil_(nil) {
    assert(obj != nullptr);
}

TypeMemPtr *TypeMemPtr::make(TypeStruct *obj, bool nil) {
  return dynamic_cast<TypeMemPtr*>((alloc.new_object<TypeMemPtr>(obj, nil))->intern());
}

Type* TypeMemPtr::makeZero() {
    return NULLPTR();
}
TypeMemPtr* TypeMemPtr::make_from(TypeStruct* obj) {
    return make(obj, nil_);
}

TypeMemPtr* TypeMemPtr::make_from(bool nil) {
    return nil == nil_ ? this : make(obj_, nil);
}

TypeMemPtr* TypeMemPtr::lub() {
    return make(obj_->lub(), false);
}

bool TypeMemPtr::isFinal() {
    return obj_->isFinal();
}
TypeMemPtr* TypeMemPtr::makeR0() {
    return make_from(obj_->makeR0());
}

bool TypeMemPtr::isConstant() {
    return this == NULLPTR();
}
bool TypeMemPtr::isHighOrConst() {
    return this == TOP() || this == NULLPTR();
}
Type* TypeMemPtr::nonZero() {
    return VOIDPTR();
}
int TypeMemPtr::log_size() {
    return 2; // 4 byte pointers
}
TypeMemPtr *TypeMemPtr::make(TypeStruct *obj) {
    return TypeMemPtr::make(obj, false);
}

TypeMemPtr* TypeMemPtr::BOT() {
    static TypeMemPtr* bot = make(TypeStruct::BOT(), true);
    return bot;
}


TypeMemPtr* TypeMemPtr::TOP() {
    static TypeMemPtr* top =  BOT()->dual();
    return top;
}

TypeMemPtr* TypeMemPtr::NULLPTR() {
    static TypeMemPtr* NullPtr=  make(TypeStruct::TOP(), true);
    return NullPtr;

}

TypeMemPtr* TypeMemPtr::VOIDPTR() {
    static TypeMemPtr* voidptr =  NULLPTR()->dual();
    return voidptr;

}


TypeMemPtr *TypeMemPtr::test() {
    static TypeMemPtr* test =  make(TypeStruct::test(), true);
    return test;
}

void TypeMemPtr::gather(Tomi::Vector<Type *>& ts) {
    ts.push_back(TypeMemPtr::NULLPTR());
    ts.push_back(TypeMemPtr::BOT());
    ts.push_back(TypeMemPtr::test());
}

Type *TypeMemPtr::xmeet(Type *other) {
    auto* that = dynamic_cast<TypeMemPtr*>(other);
    return TypeMemPtr::make(dynamic_cast<TypeStruct*>(obj_->meet(that->obj_)), nil_ | that->nil_);
}

TypeMemPtr* TypeMemPtr::dual() {
    return TypeMemPtr::make( obj_->dual(), !nil_);
}
TypeMemPtr* TypeMemPtr::glb() {
    return make(obj_->glb(), true);
}

Type* TypeMemPtr::makeInit() {
    return nil_ ? NULLPTR() : Type::TOP();
}

int TypeMemPtr::hash() {
    if(obj_->hashCode() == -2052359012) {
        std::cerr << "Here";
    }
    if(obj_->hashCode() ^ 1024 == 639589409){
        std::cerr << "Here";
    }
    if(obj_->hashCode() == 639589409){
        std::cerr << "Here";
    }
    return static_cast<int>(obj_->hashCode() ^ (nil_ ? 1024 : 0));
}

bool TypeMemPtr::isHigh() {
    return this == TOP();
}
bool TypeMemPtr::eq(Type *other) {
    if (other == this) return true;
    auto* that = dynamic_cast<TypeMemPtr*>(other);
    return obj_ == that->obj_ && nil_ == that->nil_;
}

std::ostringstream &TypeMemPtr::print_1(std::ostringstream &builder) {
    if(this == NULLPTR()) {builder << "NULLPTR"; return builder;}
    if(this == VOIDPTR()) {builder << "*void"; return builder;}
    builder <<  obj_->print_1(builder).str() << "*" << (nil_ ? "!" : "");
    return builder;
}

std::string TypeMemPtr::str() {
    if(this == NULLPTR()) return "NULLPTR";
    if(this == VOIDPTR()) return "*void";
    return "*" + obj_->str() + (nil_ ? "?" : "");
}
