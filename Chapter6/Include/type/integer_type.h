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
   * if not constant then 0=bottom, 1=top.
   * */
  bool is_con_;
  bool isTop();
  bool isBot();
  explicit TypeInteger(bool is_con, long con);
  bool isConstant() override;
  std::ostringstream &_print(std::ostringstream &builder) override;
  std::string toString();
  Type *meet(Type *other) override;
  long value();
  bool equals(TypeInteger *);

private:
  long con_;
};
#endif