#ifndef INTEGER_TYPE_H
#define INTEGER_TYPE_H
#include "../type/type.h"

class TypeInteger: public Type {
public:
  static TypeInteger* ZERO;
  static TypeInteger* constant(long con);
  explicit TypeInteger(long con);
  bool isConstant() override;
  std::ostringstream& _print(std::ostringstream& builder) override;
  static TypeInteger Zero;
  std::string toString();
  long value();
  bool equals(TypeInteger*);
private:
  long con_;
};
#endif