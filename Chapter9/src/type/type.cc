#include "../../Include/type/type.h"

bool Type::isHighOrConst() { return type_ == TTOP || type_ == TXCTRL; }
bool Type::isConstant() {return false;}

bool Type::isSimple() { return type_ < TSIMPLE; }
std::ostringstream &Type::print_1(std::ostringstream &builder) {
  if (isSimple())
    builder << STRS[type_];
  return builder;
}

Type Type::BOTTOM = Type(TBOT).intern();
Type Type::TOP = Type(TTOP).intern();
Type Type::CONTROL = Type(TCTRL).intern();
Type Type::XCONTROL = Type(TXCTRL).intern();

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
  return &BOTTOM;
}
std::string Type::toString() { return print_1(builder).str(); }

Type *Type::xmeet(Type *t) {
  assert(isSimple());
  if (type_ == TBOT || t->type_ == TTOP)
    return this;
  if (type_ == TTOP || t->type_ == TBOT)
    return t;
  if (!t->isSimple())
    return &BOTTOM;
  return ((type_ == TCTRL) || (t->type_ == TCTRL)) ? &CONTROL : &XCONTROL;
}

int Type::hash() {return type_;}
bool Type::operator==(Type &o) {
  if(o == this) return true;

  if(type_ != o.type_) return false;
  return eq(t);
}

bool Type::eq(Type *t) {
  return true;
}
int Type::hashCode() {
  if(hash_ != 0) return hash;
  hash_ = hash();
  if(hash_ == 0) hash_ = 0xDEADBEEF;
  return hash_;
}

bool Type::isa(Type *t) {
  return meet(t) == t;
}

Type* Type::join(Type *t) {
  if(this == t) return this;
  return dual()->meet(t.dual()).dual();
}

Type* Type::dual() {
  switch(type_) {
  case TBOT: return TOP;
  case TTOP: return BOTTOM;
  case TCTRL: return XCONTROL;
  case TXCTRL: return CONTROL;
  default: {throw std::runtime_error("Should not reach here");}
  }
}
template <typename T>
T Type::intern() {
  T nnn = INTERN[this];
  if(nnn == nullptr) {
    INTERN[this] = this;
    return nnn;
  }
}