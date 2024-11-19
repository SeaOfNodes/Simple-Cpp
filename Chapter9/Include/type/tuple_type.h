#ifndef TUPLE_TYPE_H
#define TUPLE_TYPE_H
#include "../../Include/type/type.h"
#include <initializer_list>

class TypeTuple : public Type {
public:
  Tomi::Vector<Type *> types_;

  TypeTuple(std::initializer_list<Type *> types);

  Type *xmeet(Type *other);

  // DEBUG INFO FOR DEBUGGER
  std::string ToString() override;

  std::ostringstream &print_1(std::ostringstream &builder) override;
  static TypeTuple* make(std::initializer_list<Type*>);

  bool eq(Type*t) override;
  int hash() override;

  static TypeTuple *IF_BOTH();
  static TypeTuple *IF_NEITHER();
  static TypeTuple *IF_TRUE();
  static TypeTuple *IF_FALSE();
};
#endif
