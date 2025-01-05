#ifndef TUPLE_TYPE_H
#define TUPLE_TYPE_H
#include "../../Include/type/type.h"
#include <initializer_list>

class TypeTuple : public Type {
public:
  Tomi::Vector<Type *> types_;

  TypeTuple(std::initializer_list<Type *> types);
  explicit TypeTuple(Tomi::Vector<Type *> &types);

  Type *xmeet(Type *other) override;
  static TypeTuple* TEST();
  static TypeTuple* START();

  // DEBUG INFO FOR DEBUGGER
  std::string ToString() override;
  static void gather(Tomi::Vector<Type *> &ts);

  std::ostringstream &print_1(std::ostringstream &builder) override;
  static TypeTuple *make(std::initializer_list<Type *>);
  static TypeTuple *make(Tomi::Vector<Type *>);

  std::ostringstream& typeName(std::ostringstream &builder) override;

  Type* glb() override;
  std::string str() override;
  bool eq(Type *t) override;
  int hash() override;

  static TypeTuple *IF_BOTH();
  static TypeTuple *IF_NEITHER();
  static TypeTuple *IF_TRUE();
  static TypeTuple *IF_FALSE();
};
#endif
