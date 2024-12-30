#ifndef INTEGER_TYPE_H
#define INTEGER_TYPE_H
#include "../type/type.h"
#include "errno.h"


class TypeInteger : public Type {
public:
  static TypeInteger* ZERO();
  static TypeInteger* TOP();
  static TypeInteger* BOT();
  static TypeInteger* U1();
  static TypeInteger* BOOL();
  static TypeInteger* FALSE();
  static TypeInteger* TRUE();

/**
 * Describes an integer *range* - everything from min to max; both min and
 * max are inclusive.  If min==max, this is a constant.
 *
 * If min <= max, this is a  below center (towards bottom).
 * If min >  max, this is an above center (towards top).
 */
  static TypeInteger* I8();
  static TypeInteger* I16();
  static TypeInteger* I32();

  Type*glb() override;

  static TypeInteger* U8();
  static TypeInteger* U16();
  static TypeInteger* U32();

  static TypeInteger* constant(long con);

  /*
   * The constant value of
   * if not constant then 1=bottom, 0=bottom.
   * */
  bool is_con_{};

  long min_{};
  long max_{};

 explicit TypeInteger(long min, long max);

  static TypeInteger* make(bool is_con, long con);
  static TypeInteger* make(long hi, long max);


  bool isConstant() override;
  bool isHighOrConst() override;

  bool isHigh() override;

  static void gather(Tomi::Vector<Type *> &ts);
    // AND-mask of forced zeros.  e.g. unsigned types will return their mask;
    // u8 will return 0xFF.  But also a range of 16-18 (0x10-0x12) will return
    // 0x13 - no value in the range {16,17,18} will allow bit 0x04 to be set.
  long mask();
  std::ostringstream &print_1(std::ostringstream &builder) override;

  // DEBUG INFO FOR DEBUGGER
  std::string ToString() override;
  Type *xmeet(Type *other) override;

  Type* dual() override;
  std::string str() override;
  TypeInteger* makeInit() override;
  std::ostringstream& typeName(std::ostringstream& builder) override;

  long value();
  bool eq(Type *) override;

  int hash() override;
};
#endif