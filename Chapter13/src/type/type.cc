#include "../../Include/type/type.h"
#include "../../Include/type/field.h"
#include "../../Include/type/integer_type.h"
#include "../../Include/type/type_mem_ptr.h"
#include "../../Include/type/tuple_type.h"
#include <iostream>

int Type::get_hash() { return hash_; }
bool Type::isHighOrConst() { return type_ == TTOP || type_ == TXCTRL; }
bool Type::isConstant() { return false; }

bool Type::isSimple() { return type_ < TSIMPLE; }
std::ostringstream &Type::print_1(std::ostringstream &builder) {
  if (isSimple())
    builder << STRS[type_];
  return builder;
}

Type* Type::makeInit() {
    return nullptr;
}

Type *Type::CONTROL() {
  static Type CONTROL = Type(TCTRL);
  // intern on the persistent object
  return CONTROL.intern();
}

Type *Type::XCONTROL() {
  static Type XCONTROL = Type(TXCTRL);
  // intern on the persistent object
  return XCONTROL.intern();
}

Type *Type::TOP() {
  static Type TOP = Type(TTOP);
  // intern on the persistent object
  return TOP.intern();
}

Type *Type::BOTTOM() {
  static Type BOTTOM = Type(TBOT);
  // intern on the persistent object
  return BOTTOM.intern();
}

Type *Type::intern() {
  static Tomi::HashMap<Type *, Type *> INTERN;
  Type **nnn = INTERN.get(this);

  if (nnn == nullptr) {
    INTERN.put(this, this); // call hashCode as a side-effect
    return this;
  }
  Type* nptr = *nnn;

//  if(nptr->hash_ == 9876545) {
//      std::cerr << "here";
//  }
  return *nnn;
}
Type::Type(unsigned int type) : type_(type) {}
Type *Type::meet(Type *other) {
  // Shortcut for the self case
  if (other == this)
    return this;
  // Same-type is always safe in the subclasses
  if (type_ == other->type_)
    return xmeet(other);
  // Reverse; xmeet 2nd arg is never "is_simple" and never equal to "this".
  if (isSimple())
    return xmeet(other);
  if (other->isSimple())
    return other->xmeet(this);
  return Type::BOTTOM();
}
// O(1)
std::string Type::ToString() {
  std::ostringstream os;
  os << "hash: " << get_hash();
  os << "type: ";
  switch (type_) {
  case 0:
    os << "TBOT";
    break;
  case 1:
    os << "TTOP";
    break;
  case 2:
    os << "TCTRL";
    break;
  case 3:
    os << "TXCTRL";
    break;
  case 4:
    os << "TSIMPLE";
    break;
  case 5:
    os << "TINT";
    break;
  case 6:
    os << "TTUPLE";
    break;
  }
  return os.str();
}

Type *Type::xmeet(Type *t) {
  assert(isSimple());
  if (type_ == TBOT || t->type_ == TTOP)
    return this;
  if (type_ == TTOP || t->type_ == TBOT)
    return t;
  if (!t->isSimple())
    return BOTTOM();
  return ((type_ == TCTRL) || (t->type_ == TCTRL)) ? CONTROL() : XCONTROL();
}

int Type::hash() { return type_; }

std::string Type::str() {
    return STRS[type_];
}

std::ostringstream& Type::typeName(std::ostringstream& builder) {
    return print_1(builder);
}
bool Type::operator==(Type& o) {
  if (&o == this)
    return true;
  if (!dynamic_cast<Type *>(&o))
    return false;
  if (type_ != o.type_)
    return false;
  return eq(&o);
}

Tomi::Vector<Type *> Type::gather() {
    Tomi::Vector<Type*> ts;
    ts.push_back(BOTTOM());
    ts.push_back(CONTROL());
    Field::gather(ts);
    TypeInteger::gather(ts);
    TypeMemPtr::gather(ts);
    TypeStruct::gather(ts);
    TypeTuple::gather(ts);
    size_t sz = ts.size();
    for(size_t i = 0; i < sz; i++) {
        ts[i] = ts[i]->intern();
    }
    return ts;

}
bool Type::eq(Type *t) { return true; }

unsigned int Type::hashCode() {
  if (hash_ != 0)
    return hash_;
  hash_ = hash();
  if (hash_ == 0)
    hash_ = 0xDEADBEEF;
  return hash_;
}

bool Type::isa(Type *t) {
    return meet(t) == t;
}

Type *Type::join(Type *t) {
  if (this == t)
    return this;
  return dual()->meet(t->dual())->dual();
}

Type *Type::dual() {
  switch (type_) {
  case TBOT:
    return TOP();
  case TTOP:
    return BOTTOM();
  case TCTRL:
    return XCONTROL();
  case TXCTRL:
    return CONTROL();
  default: {
    throw std::runtime_error("Should not reach here");
  }
  }
}

// Part of hashmap interface see in header.
unsigned long long Tomi::hash<Type *>::operator()(Type *val) {
  return val->hashCode();
}

Type *Type::glb() {
    return type_ == TCTRL ? XCONTROL() : BOTTOM();
}