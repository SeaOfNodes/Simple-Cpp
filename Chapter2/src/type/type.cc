#include "../../Include/type/type.h"

bool Type::isConstant() { return type_ == TTOP; }

bool Type::isSimple() { return type_ < TSIMPLE; }
std::ostringstream &Type::_print(std::ostringstream& builder) {
  if(isSimple())  builder << STRS[type_];
  return builder;
}

Type Type::BOTTOM = Type(TBOT);

Type::Type(unsigned int type) : type_(type){}

std::string Type::toString() {
  return _print(builder).str();
}