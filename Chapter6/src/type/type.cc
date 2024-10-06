#include "../../Include/type/type.h"

bool Type::isConstant() { return type_ == TTOP || type_ == TXCTRL; }

bool Type::isSimple() { return type_ < TSIMPLE; }
std::ostringstream &Type::print_1(std::ostringstream &builder) {
  if (isSimple())
    builder << STRS[type_];
  return builder;
}

Type Type::BOTTOM = Type(TBOT);
Type Type::TOP = Type(TTOP);
Type Type::CONTROL = Type(TCTRL);
Type Type::XCONTROL = Type(TXCTRL);

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
