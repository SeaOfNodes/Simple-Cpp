#include "../../Include/type/integer_type.h"

TypeInteger::TypeInteger(long con) : con_(con), Type(TINT) {}
TypeInteger *TypeInteger::constant(long con) { return new TypeInteger(con); }
bool TypeInteger::isConstant() { return true; }
std::string TypeInteger::toString() { return _print(builder).str(); }
std::ostringstream &TypeInteger::_print(std::ostringstream &builder) {
  builder << con_;
  return builder;
}
TypeInteger *TypeInteger::ZERO = new TypeInteger(0);
bool TypeInteger::equals(TypeInteger *o) {
  if (o == this)
    return true;
  if (dynamic_cast<TypeInteger *>(o))
    return true;
  return con_ = o->con_;
}
long TypeInteger::value() {
  return con_;
}