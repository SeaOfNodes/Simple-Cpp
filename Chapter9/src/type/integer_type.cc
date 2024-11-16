#include "../../Include/type/integer_type.h"

TypeInteger::TypeInteger(bool is_con, long con)
    : con_(con), is_con_(is_con), Type(TINT) {}

TypeInteger *TypeInteger::constant(long con) { return make(true, con); }

bool TypeInteger::isConstant() { return is_con_; }
bool TypeInteger::isHighOrConst() { return is_con_ || con_ == 0; }

std::string TypeInteger::toString() { return print_1(builder).str(); }

std::ostringstream &TypeInteger::print_1(std::ostringstream &builder) {
  if(this == TOP) builder << "IntTop";
  if(this == BOT) builder << "IntBot";
  builder << con_;
  return builder;
}

// Why the values the way they are is this because idealise optimisation
TypeInteger *TypeInteger::BOT = make(false, 1);
TypeInteger *TypeInteger::TOP = make(false, 0);
TypeInteger *TypeInteger::ZERO = make(true, 0);

bool TypeInteger::equals(TypeInteger *o) {
  if (o == this)
    return true;
  if (dynamic_cast<TypeInteger *>(o))
    return true;
  return con_ = o->con_;
}

Type *TypeInteger::xmeet(Type *other) {
  // Invariant from caller: 'this' != 'other' and same class (TypeInteger)
  TypeInteger *i = dynamic_cast<TypeInteger *>(other);
  if (this == BOT)
    return this;

  if (i == BOT)
    return i;
  // TOP loses
  if (i == TOP)
    return this;
  if (this == TOP)
    return i;

  // Since both are constants, and are never equals (contract) unequals
  // constants fall to bottom
  return BOT;
}

Type *TypeInteger::dual() {
  if(isConstant()) return this; // Constants are a self-dual
  return con_ == 0? BOT : TOP;
}
TypeInteger *TypeInteger::make(bool is_con, long con) {
  return (new TypeInteger(is_con, con))->intern<TypeInteger>();
}

long TypeInteger::value() { return con_; }
int TypeInteger::hash() { return con_ ^ (is_con_ ? 0 : 0x4000); }
bool TypeInteger::eq(Type *t) {
  TypeInteger *i = dynamic_cast<TypeInteger *>(t);
  return con_ == i->con_ && is_con_ == i->is_con_;
}