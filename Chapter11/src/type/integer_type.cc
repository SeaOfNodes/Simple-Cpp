#include "../../Include/type/integer_type.h"

std::ostream &operator<<(std::ostream &os, TypeInteger &type_integer) {
  os << "TypeInteger: " << type_integer.value();
  return os;
}
TypeInteger::TypeInteger(bool is_con, long con)
    : con_(con), is_con_(is_con), Type(TINT) {}

TypeInteger *TypeInteger::constant(long con) { return make(true, con); }

bool TypeInteger::isConstant() { return is_con_; }
bool TypeInteger::isHighOrConst() { return is_con_ || con_ == 0; }

std::string TypeInteger::ToString() {
  std::ostringstream os;
  os << "TypeInteger: " << value();
  return os.str();
}
std::ostringstream& TypeInteger::typeName(std::ostringstream& builder) {
    if(this == TOP()) builder << "IntTop";
    if(this == BOT()) builder << "IntBot";
    else {
        builder << "Int";
    }
    return builder;
}

void TypeInteger::gather(Tomi::Vector<Type *> &ts) {
    ts.push_back(ZERO());
    ts.push_back(BOT());
}

std::ostringstream &TypeInteger::print_1(std::ostringstream &builder) {
  if (this == TOP())
    builder << "IntTop";
  if (this == BOT())
    builder << "IntBot";
  builder << con_;
  return builder;
}

// Why the values the way they are is this because idealise optimisation
TypeInteger *TypeInteger::TOP() {
   return make(false, 0);
}
TypeInteger *TypeInteger::BOT() {
    return make(false, 1);
}
TypeInteger *TypeInteger::ZERO () {
    return make(true, 0);
}
TypeInteger * TypeInteger::makeInit() {
    return ZERO();
}

std::string TypeInteger::str() {
    if(this == TOP()) return "~int";
    if(this == BOT()) return "_int";
    return std::to_string(con_);
}
Type *TypeInteger::xmeet(Type *other) {
  // Invariant from caller: 'this' != 'other' and same class (TypeInteger)
  auto *i = dynamic_cast<TypeInteger *>(other);
  if (this == BOT())
    return this;

  if (i == BOT())
    return i;
  // TOP loses
  if (i == TOP())
    return this;
  if (this == TOP())
    return i;

  // Since both are constants, and are never equals (contract) unequals
  // constants fall to bottom
  return BOT();
}

Type *TypeInteger::dual() {
  if (isConstant())
    return this; // Constants are a self-dual
  return con_ == 0 ? BOT() : TOP();
}
TypeInteger *TypeInteger::make(bool is_con, long con) {
  return dynamic_cast<TypeInteger *>((alloc.new_object<TypeInteger>(is_con, con))->intern());
}

long TypeInteger::value() { return con_; }
int TypeInteger::hash() { return con_ ^ (is_con_ ? 0 : 0x4000); }
bool TypeInteger::eq(Type *t) {
  auto *i = dynamic_cast<TypeInteger *>(t);
  return con_ == i->con_ && is_con_ == i->is_con_;
}