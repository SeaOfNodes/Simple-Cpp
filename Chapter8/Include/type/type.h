#ifndef TYPE_H
#define TYPE_H
#include <cassert>
#include <iostream>
#include <sstream>

class Type {
public:
  std::ostringstream builder;
  Type() = default;
  static constexpr unsigned int TBOT = 0;
  static constexpr unsigned int TTOP = 1;
  static constexpr unsigned int TCTRL = 2;
  static constexpr unsigned int TXCTRL = 3;
  // end of simple types
  static constexpr unsigned int TSIMPLE = 4;
  // All integers; see TypeInteger
  static constexpr unsigned int TINT = 5;
  // Tuples; finite collections of unrelated Types, kept in parallel
  static constexpr unsigned int TTUPLE = 6;

  unsigned int type_;

  static Type BOTTOM;
  static Type TOP;
  static Type CONTROL;
  static Type XCONTROL;

  virtual bool isConstant();

  const char *STRS[4] = {"Bot", "Top", "Ctrl", "~Ctrl"};
  virtual std::ostringstream &print_1(std::ostringstream &builder);

  bool isSimple();
  std::string toString();

  virtual Type *meet(Type *other);
  virtual Type *xmeet(Type *other);

protected:
  explicit Type(unsigned int type);
};
#endif