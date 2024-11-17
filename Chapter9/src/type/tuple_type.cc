#include "../../Include/type/tuple_type.h"
#include <iostream>


TypeTuple::TypeTuple(std::initializer_list<Type *> types) : Type(TTUPLE) {
  for (auto type : types) {
    types_.push_back(type);
  }
}

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
  throw std::runtime_error("Meet on Tuple type not yet implemented!");
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
  return static_cast<TypeTuple *>((new TypeTuple(types))->intern());
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
TypeTuple *TypeTuple::IF_BOTH = make({Type::CONTROL, Type::CONTROL});

TypeTuple *TypeTuple::IF_NEITHER = make({Type::XCONTROL, Type::XCONTROL});

TypeTuple *TypeTuple::IF_TRUE = make({Type::CONTROL, Type::XCONTROL});

TypeTuple *TypeTuple::IF_FALSE = make({Type::XCONTROL, Type::CONTROL});
