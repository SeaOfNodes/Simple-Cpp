#ifndef INTEGER_TYPE_H
#define INTEGER_TYPE_H
#include "../type/type.h"
#include "errno.h"

class TypeInteger : public Type {
public:
  static TypeInteger ZERO;
  static TypeInteger TOP;
  static TypeInteger BOT;
  static TypeInteger *constant(long con);

  /*
   * The constant value of
   * if not constant then 1=bottom, 0=bottom.
   * */
  bool is_con_;

  static TypeInteger make(bool is_con, long con);

  explicit TypeInteger(bool is_con, long con);
  bool isConstant() override;
  bool isHighOrConst() override;

  std::ostringstream &print_1(std::ostringstream &builder) override;
  std::string toString();
  Type *xmeet(Type *other) override;

  long value();
  bool equals(TypeInteger *);

  int hash() override;
  bool eq(Type*t) override;

private:
  long con_;
};
#endif