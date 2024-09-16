#ifndef TUPLE_TYPE_H
#define TUPLE_TYPE_H
#include "../../Include/type/type.h"
#include <initializer_list>
#include <vector>

class TypeTuple : public Type {
  std::vector<Type *> types_;
  TypeTuple(std::initializer_list<Type> types);

  Type *meet() override;
  std::ostringstream print_1(std::ostringstream &builder);
};
#endif