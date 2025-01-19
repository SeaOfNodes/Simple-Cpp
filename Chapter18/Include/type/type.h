#ifndef TYPE_H
#define TYPE_H
#include "../tomi.h"
#include <cassert>
#include <iostream>
#include <memory>
#include <memory_resource>
#include <optional>
#include <sstream>

class Node;
// arena allocator(static initialisation)
inline std::optional<std::pmr::monotonic_buffer_resource> mem{std::in_place,
                                                              1024};
inline std::pmr::polymorphic_allocator<Node> alloc(&*(mem));
// arena allocator end

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
  static constexpr unsigned int TNIL = 4; // low null of all flavors
  static constexpr unsigned int TXNIL = 5;  // high or choice null

  // end of simple types
  static constexpr unsigned int TSIMPLE = 6;  // End of the Simple Types
  // All integers; see TypeInteger
  static constexpr unsigned int TPTR = 7; //  // All nil-able scalar values
    static constexpr unsigned int TINT = 8;  // All nil-able scalar values
  // Tuples; finite collections of unrelated Types, kept in parallel
  static constexpr unsigned int TFLT = 9;
  static constexpr unsigned int TMEMPTR = 10;
  static constexpr unsigned int TFUNPTR =  11;
  static constexpr unsigned int TTUPLE = 12;
  static constexpr unsigned int TMEM = 13;
  static constexpr unsigned int TSTRUCT = 14;
  static constexpr unsigned int TFLD = 15;
  static constexpr unsigned int TRPC = 16;

  unsigned int type_{};

  bool shallowISA(Type *t);

  static void reset();
  static Type *BOTTOM();
  static Type *TOP();
  static Type *CONTROL();
  static Type *XCONTROL();
  static Type*NIL();
  static Type*XNIL();
  virtual Type* nonZero();
// ----------------------------------------------------------

// Size in bits to hold an instance of this type.
// Sizes are expected to be between 1 and 64 bits.
// Size 0 means this either takes no space (such as a known-zero field)
// or isn't a scalar to be stored in memory.
  virtual int log_size();
  unsigned int hashCode();
  virtual int hash();

  // Is high or on the lattice centerline.
  virtual bool isHighOrConst();
  virtual bool isHigh();
  // Strict constant values, things on the lattice centerline.
  // Excludes both high and low values
  virtual bool isConstant();
  virtual std::string str();

  const char *STRS[4] = {"Bot", "Top", "Ctrl", "~Ctrl"};
  virtual std::ostringstream &print_1(std::ostringstream &builder);
  virtual std::ostringstream &gprint(std::ostringstream &builder);
  // ----------------------------------------------------------

  // Factory method which interns "this"
  Type *intern();
  virtual std::ostringstream& typeName(std::ostringstream& builder);

  virtual Type* makeInit();
  // Make a zero version of this type, 0 for integers and null for pointers.
  virtual Type* makeZero();
  // Make a non-zero version of this type, if possible.  Integers attempt to
  // exclude zero from their range and pointers become not-null.


  // Is forward-reference
  virtual bool isFRef();

  virtual bool isFinal();
 // Make all reachable struct Fields final
 virtual Type* makeR0();

  virtual bool eq(Type *t);

  static Tomi::Vector<Type*> gather();
  // to avoid collisions with INTERN
  int get_hash();
  bool operator==(Type&);

  bool isSimple();
  // DEBUG INFO FOR DEBUGGER
  virtual std::string ToString();

  bool isa(Type *t);
    /**
   * Compute greatest lower bound in the lattice
   */
   virtual Type* glb();
    /** Compute least upper bound in the lattice */
   virtual Type* lub();

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