#ifndef TYPE_H
#define TYPE_H
#include "../tomi.h"
#include <sstream>


// Custom hashing for Type:
class Type;

template <> struct Tomi::hash<Type *> {
  unsigned long long operator()(Type *val);
};

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

  unsigned int type_{};

  static Type *BOTTOM();
  static Type *TOP();
  static Type *CONTROL();
  static Type *XCONTROL();

  int hashCode();
  virtual int hash();

  // Is high or on the lattice centerline.
  virtual bool isHighOrConst();
  // Strict constant values, things on the lattice centerline.
  // Excludes both high and low values
  virtual bool isConstant();

  const char *STRS[4] = {"Bot", "Top", "Ctrl", "~Ctrl"};
  virtual std::ostringstream &print_1(std::ostringstream &builder);
  // ----------------------------------------------------------

  // Factory method which interns "this"
  Type *intern();

  virtual bool eq(Type *t);

  // to avoid collisions with INTERN
  int get_hash();
  bool operator==(Type *);

  bool isSimple();
  // DEBUG INFO FOR DEBUGGER
  virtual std::string ToString();

  bool isa(Type *t);
  // Our lattice is defined with a MEET and a DUAL.
  // JOIN is dual of meet of both duals.
  virtual Type *join(Type *t);
  virtual Type *dual();

  virtual Type *meet(Type *other);
  virtual Type *xmeet(Type *other);

protected:
  explicit Type(unsigned int type);

private:
  int hash_{}; // Hash cache; not-zero when set.
};

#endif