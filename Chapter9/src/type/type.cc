#include "../../Include/type/type.h"

bool Type::isHighOrConst() { return type_ == TTOP || type_ == TXCTRL; }
bool Type::isConstant() { return false; }

bool Type::isSimple() { return type_ < TSIMPLE; }
std::ostringstream &Type::print_1(std::ostringstream &builder) {
  if (isSimple())
    builder << STRS[type_];
  return builder;
}

Tomi::HashMap<Type *, Type *> Type::INTERN = Tomi::HashMap<Type *, Type *>();
Type *Type::BOTTOM = Type(TBOT).intern<Type>();
Type *Type::TOP = Type(TTOP).intern<Type>();
Type *Type::CONTROL = Type(TCTRL).intern<Type>();
Type *Type::XCONTROL = Type(TXCTRL).intern<Type>();

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
  return BOTTOM;
}
std::string Type::toString() { return print_1(builder).str(); }

Type *Type::xmeet(Type *t) {
  assert(isSimple());
  if (type_ == TBOT || t->type_ == TTOP)
    return this;
  if (type_ == TTOP || t->type_ == TBOT)
    return t;
  if (!t->isSimple())
    return BOTTOM;
  return ((type_ == TCTRL) || (t->type_ == TCTRL)) ? CONTROL : XCONTROL;
}

int Type::hash() { return type_; }

bool Type::operator==(Type *o) {
  if (o == this)
    return true;
  if (!dynamic_cast<Type *>(o))
    return false;
  if (type_ != o->type_)
    return false;
  return eq(o);
}

bool Type::eq(Type *t) { return true; }

int Type::hashCode() {
  if (hash_ != 0)
    return hash_;
  hash_ = hash();
  if (hash_ == 0)
    hash_ = 0xDEADBEEF;
  return hash_;
}

bool Type::isa(Type *t) { return meet(t) == t; }

Type *Type::join(Type *t) {
  if (this == t)
    return this;
  return dual()->meet(t->dual())->dual();
}

Type *Type::dual() {
  switch (type_) {
  case TBOT:
    return TOP;
  case TTOP:
    return BOTTOM;
  case TCTRL:
    return XCONTROL;
  case TXCTRL:
    return CONTROL;
  default: {
    throw std::runtime_error("Should not reach here");
  }
  }
}

// Part of hashmap interface see in header.
unsigned long long Tomi::hash<Type *>::operator()(Type *val) {
  return val->hashCode();
}
