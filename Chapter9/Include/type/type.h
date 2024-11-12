#ifndef TYPE_H
#define TYPE_H
#include <cassert>
#include <iostream>
#include <sstream>
#include <unordered_map>

class Type {
public:
  static std::unordered_map<Type *, Type *> INTERN;
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

  static Type *BOTTOM;
  static Type *TOP;
  static Type *CONTROL;
  static Type *XCONTROL;

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
  template <typename T> T *intern() {
    T *nnn = dynamic_cast<T *>(INTERN[this]);
    if (nnn == nullptr) {
      INTERN[this] = this;
      return static_cast<T *>(this);
    }
    return nnn;
  }
  virtual bool eq(Type *t);

  bool operator==(Type *);

  bool isSimple();
  std::string toString();

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
  int hash_; // Hash cache; not-zero when set.
};
#endif