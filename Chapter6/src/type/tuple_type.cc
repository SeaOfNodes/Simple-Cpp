#include "../../Include/type/tuple_type.h"

TypeTuple::TypeTuple(std::initializer_list<Type *> types) : Type(TTUPLE) {
  for (auto type : types) {
    types_.push_back(type);
  }
}

Type *TypeTuple::xmeet(Type *other) {
  throw std::runtime_error("Meet on Tuple type not yet implemented!");
}

std::ostringstream TypeTuple::print_1(std::ostringstream &builder) {
  builder << "[";
  // fence-post problem here
  for (auto t : types_) {
    t->_print(builder);
    builder << ",";
  }
  builder << "]";
}

TypeTuple TypeTuple::IF_BOTH = TypeTuple({&Type::CONTROL, &Type::CONTROL});

TypeTuple TypeTuple::IF_NEITHER = TypeTuple({&Type::XCONTROL, &Type::XCONTROL});

TypeTuple TypeTuple::IF_TRUE = TypeTuple({&Type::CONTROL, &Type::XCONTROL});

TypeTuple TypeTuple::IF_FALSE = TypeTuple({&Type::XCONTROL, &Type::CONTROL});
