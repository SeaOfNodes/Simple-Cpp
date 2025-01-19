#include "../../Include/type/type_mem_ptr.h"
#include <cassert>

TypeMemPtr::TypeMemPtr(bool nil, TypeStruct *obj) : Type(TMEMPTR, nil), obj_(obj) {
    assert(obj != nullptr);
}

TypeMemPtr *TypeMemPtr::make( bool nil, TypeStruct *obj) {
  return dynamic_cast<TypeMemPtr*>((alloc.new_object<TypeMemPtr>(nil, obj))->intern());
}

TypeMemPtr* TypeMemPtr::makeNullable() {
    return make_from(3);
}
Type* TypeMemPtr::makeZero() {
    return NULLPTR();
}
TypeMemPtr* TypeMemPtr::make_from(TypeStruct* obj) {
    return make(nil_, obj);
}

TypeMemPtr* TypeMemPtr::make_from(unsigned int nil) {
    return nil == nil_ ? this : make(nil, obj_);
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
TypeMemPtr *TypeMemPtr::makeNullable(TypeStruct *obj) {
    return TypeMemPtr::make(3, obj);
}

TypeMemPtr* TypeMemPtr::BOT() {
    static TypeMemPtr* bot = make(3, TypeStruct::BOT());
    return bot;
}


TypeMemPtr* TypeMemPtr::TOP() {
    static TypeMemPtr* top =  BOT()->dual();
    return top;
}

TypeMemPtr *TypeMemPtr::NOTBOT() {
    return make(2, TypeStruct::BOT());
}
//TypeMemPtr* TypeMemPtr::NULLPTR() {
//    static TypeMemPtr* NullPtr=  make(TypeStruct::TOP(), true);
//    return NullPtr;
//
//}
//
//TypeMemPtr* TypeMemPtr::VOIDPTR() {
//    static TypeMemPtr* voidptr =  NULLPTR()->dual();
//    return voidptr;
//
//}


TypeMemPtr *TypeMemPtr::test() {
    static TypeMemPtr* test =  make(2, TypeStruct::test());
    return test;
}

void TypeMemPtr::gather(Tomi::Vector<Type *>& ts) {
    ts.push_back(TypeMemPtr::NOTBOT());
    ts.push_back(TypeMemPtr::BOT());
    ts.push_back(TypeMemPtr::test());
}

Type *TypeMemPtr::xmeet(Type *other) {
    auto* that = dynamic_cast<TypeMemPtr*>(other);
    return TypeMemPtr::make(xmeet0(that), obj_->meet(that->obj_));
}

bool TypeMemPtr::shallowISA(Type *t) {
    TypeMemPtr*that = dynamic_cast<TypeMemPtr*>(t);
    if(!that) return false;
    if(this == that) return true;
    if(xmeet0(that) != that->nil_) return false;
    if(obj_->that->obj_) return true;
    // Shallow, do not follow matching names, just assume ok
    if(obj_->name_ == that->obj_->name_) return true;
    throw std::runtime_error("TODO!");
}
TypeMemPtr *TypeMemPtr::make(TypeStruct *obj) {
    return alloc.new_object<TypeMemPtr*>(2, obj)->intern();
}
TypeMemPtr* TypeMemPtr::dual() {
    return TypeMemPtr::make(dual0(), obj_->dual());
}
Type* TypeMemPtr::meet0() {
    return nil_ == 3 ? this: make(3, obj_);
}
TypeMemPtr* TypeMemPtr::glb() {
    return make(3, obj_->glb());
}

Type* TypeMemPtr::makeInit() {
    return nil_ ? NULLPTR() : Type::TOP();
}

int TypeMemPtr::hash() {
/*    if(obj_->hashCode() == -2052359012) {
        std::cerr << "Here";
    }
    if(obj_->hashCode() ^ 1024 == 639589409){
        std::cerr << "Here";
    }
    if(obj_->hashCode() == 639589409){
        std::cerr << "Here";
    }*/
   return obj_->hashCode() ^ TypeNil::hash();
}

bool TypeMemPtr::isHigh() {
    return this == TOP();
}
bool TypeMemPtr::eq(Type *other) {
    TypeMemPtr*ptr = dynamic_cast<TypeMemPtr*>(t);
    return obj_ == ptr->obj_ && TypeNil::eq(ptr);
}

std::ostringstream &TypeMemPtr::print_1(std::ostringstream &builder) {
    if(this == NOTBOT()) {builder << "*void"; return builder;}
    if(this == BOT()) {builder << "*void?"; return builder;}
    builder <<  obj_->print_1(builder).str() << "*" << (nil_ ? "!" : "");
    return builder;
}

std::string TypeMemPtr::gprint(std::ostringstream&builder) {
    if(this == NOTBOT()) {builder << "*void"; return builder;}
    if(this == BOT()) {builder << "*void?"; return builder;}
    builder << x();
    obj_->gprint(builder);
    builder << "*";
    builder << q();
}
