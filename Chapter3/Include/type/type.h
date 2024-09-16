#ifndef TYPE_H
#define TYPE_H
#include <iostream>
#include<sstream>

class Type {
public:
  std::ostringstream builder;
  Type() = default;
  static constexpr unsigned int TBOT = 0;
  static constexpr unsigned int TTOP = 1;
  static constexpr unsigned int TSIMPLE = 2;
  static constexpr unsigned int TINT = 3;
  unsigned int type_;

  static Type BOTTOM;

  virtual bool isConstant();

  const char* STRS[2] = {"Bot", "Top"};
  virtual std::ostringstream& _print(std::ostringstream& builder);

  bool isSimple();
  std::string toString();
protected:
  explicit Type(unsigned int type);
};
#endif