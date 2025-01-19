#ifndef TUPLE_TYPE_H
#define TUPLE_TYPE_H
#include "../../Include/type/type.h"
#include <initializer_list>

class TypeTuple : public Type {
public:
  Tomi::Vector<Type *> types_;

  bool types_null{false};
  TypeTuple(std::initializer_list<Type *> types);
  explicit TypeTuple(Tomi::Vector<Type *> &types);

  Type *xmeet(Type *other) override;

  std::ostringstream& gprint(std::ostringstream &builder) override;

  TypeTuple* dual() override;
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

  static TypeTuple* BOT();
  static TypeTuple* TOP();

  static TypeTuple* TEST();
  static TypeTuple* START();

  static TypeTuple* MAIN();
  static TypeTuple* RET();


  static TypeTuple *IF_BOTH();
  static TypeTuple *IF_NEITHER();
  static TypeTuple *IF_TRUE();
  static TypeTuple *IF_FALSE();
};
#endif
