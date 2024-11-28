#include "../../Include/type/tuple_type.h"
#include <iostream>

TypeTuple::TypeTuple(std::initializer_list<Type *> types) : Type(TTUPLE) {
  for (auto type : types) {
    types_.push_back(type);
  }
}
TypeTuple::TypeTuple(Tomi::Vector<Type *> &types) { types_ = types; }

// O(N)
std::string TypeTuple::ToString() {
  std::ostringstream os;
  os << "Types of the tuple: ";
  for (Type *type : types_) {
    os << type->ToString();
  }
  return os.str();
}
Type *TypeTuple::xmeet(Type *other) {
  auto *tt = dynamic_cast<TypeTuple *>(other);
  assert(types_.size() == tt->types_.size());
  Tomi::Vector<Type *> ts;
  int cnt = 0;
  for (auto item : types_) {
    ts.push_back(item->meet(tt->types_[cnt]));
    cnt++;
  }
  return make(ts);
}

std::ostringstream &TypeTuple::print_1(std::ostringstream &builder) {
  builder << "[";
  // fence-post problem here
  for (auto t : types_) {
    t->print_1(builder);
    builder << ",";
  }
  builder << "]";
  return builder;
}

int TypeTuple::hash() {
  int sum = 0;
  for (Type *type : types_) {
    sum ^= type->hashCode();
  }
  return sum;
}
TypeTuple *TypeTuple::make(std::initializer_list<Type *> types) {
  return static_cast<TypeTuple *>((alloc.new_object<TypeTuple>(types))->intern());
}
TypeTuple *TypeTuple::make(Tomi::Vector<Type *> types) {
  return static_cast<TypeTuple *>((alloc.new_object<TypeTuple>(types))->intern());
}

bool TypeTuple::eq(Type *t) {
  TypeTuple *tt = (TypeTuple *)(t);
  if (types_.size() != tt->types_.size())
    return false;
  for (int i = 0; i < types_.size(); i++) {
    if (types_[i] != tt->types_[i])
      return false;
  }
  return true;
}
TypeTuple *TypeTuple::IF_BOTH() {
  static TypeTuple IF_BOTH_INSTANCE({Type::CONTROL(), Type::CONTROL()});
  return &IF_BOTH_INSTANCE;
}

TypeTuple *TypeTuple::IF_NEITHER() {
  static TypeTuple IF_NEITHER_INSTANCE({Type::XCONTROL(), Type::XCONTROL()});
  return &IF_NEITHER_INSTANCE;
}

TypeTuple *TypeTuple::IF_TRUE() {
  static TypeTuple IF_TRUE({Type::CONTROL(), Type::XCONTROL()});
  return &IF_TRUE;
}

TypeTuple *TypeTuple::IF_FALSE() {
  static TypeTuple IF_FALSE({Type::XCONTROL(), Type::CONTROL()});
  return &IF_FALSE;
}
