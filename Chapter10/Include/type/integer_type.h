#ifndef INTEGER_TYPE_H
#define INTEGER_TYPE_H
#include "../type/type.h"
#include "errno.h"

class TypeInteger : public Type {
public:
  static TypeInteger* ZERO();
  static TypeInteger* TOP();
  static TypeInteger* BOT();
  static TypeInteger* constant(long con);

  /*
   * The constant value of
   * if not constant then 1=bottom, 0=bottom.
   * */
  bool is_con_{};

  static TypeInteger* make(bool is_con, long con);

  explicit TypeInteger(bool is_con, long con);
  bool isConstant() override;
  bool isHighOrConst() override;

  std::ostringstream &print_1(std::ostringstream &builder) override;

  // DEBUG INFO FOR DEBUGGER
  std::string ToString() override;
  Type *xmeet(Type *other) override;

  Type* dual() override;

  TypeInteger* makeInit() override;
  long value();
  bool eq(Type *) override;

  int hash() override;
private:
  long con_;
};
#endif