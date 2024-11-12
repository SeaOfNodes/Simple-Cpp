#include "../../Include/type/tuple_type.h"

TypeTuple::TypeTuple(std::initializer_list<Type *> types) : Type(TTUPLE) {
  for (auto type : types) {
    types_.push_back(type);
  }
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

TypeTuple *TypeTuple::IF_BOTH = new TypeTuple({Type::CONTROL, Type::CONTROL});

TypeTuple *TypeTuple::IF_NEITHER =
    new TypeTuple({Type::XCONTROL, Type::XCONTROL});

TypeTuple *TypeTuple::IF_TRUE = new TypeTuple({Type::CONTROL, Type::XCONTROL});

TypeTuple *TypeTuple::IF_FALSE = new TypeTuple({Type::XCONTROL, Type::CONTROL});
