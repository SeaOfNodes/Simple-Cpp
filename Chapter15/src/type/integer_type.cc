#include "../../Include/type/integer_type.h"
#include <limits>
#include <bitset>
#include <bit>
std::ostream &operator<<(std::ostream &os, TypeInteger &type_integer) {
    os << "TypeInteger: " << type_integer.value();
    return os;
}

int TypeInteger::log_size() {
    if(this == I8() || this == U8() || this == BOOL()) return 0; // 1<<0 == 1 bytes
    if(this == I16() || this == U16()) return 1; // 1<<1 == 2 bytes
    if(this == I32() || this == U32()) return 2; // 1<<2 == 4 bytes
    if(this == BOT()) return 3;  // 1 << 3 == 8 byes for long
}

// Todo: understand this
TypeInteger *TypeInteger::nonZero() {
 if(isHigh()) return this;
 if(min_ == 0) return make(static_cast<long>(1), max_);
 if(max_ == 0) return make(min_, static_cast<long>(-1));
 return this;
}
TypeInteger::TypeInteger(long min, long max)
        : min_(min), max_(max), Type(TINT) {}

TypeInteger *TypeInteger::constant(long con) { return make(true, con); }


bool TypeInteger::isConstant() { return min_ == max_; }

bool TypeInteger::isHighOrConst() { return min_ >= max_; }

bool TypeInteger::isHigh() {
    return min_ > max_;
}
std::string TypeInteger::ToString() {
    std::ostringstream os;
    os << "TypeInteger: " << value();
    return os.str();
}

std::ostringstream &TypeInteger::typeName(std::ostringstream &builder) {
    if (this == TOP()) builder << "IntTop";
    if (this == BOT()) builder << "IntBot";
    else {
        builder << "Int";
    }
    return builder;
}

void TypeInteger::gather(Tomi::Vector<Type *> &ts) {
    ts.push_back(ZERO());
    ts.push_back(BOT());
    ts.push_back(U1());
    ts.push_back(U8());
}

std::ostringstream &TypeInteger::print_1(std::ostringstream &builder) {
    builder << str();
    return builder;
}

// Why the values the way they are is this because idealise optimisation
TypeInteger *TypeInteger::TOP() {
    return make(false, 0);
}

TypeInteger *TypeInteger::BOT() {
    return make(false, 1);
}

TypeInteger *TypeInteger::ZERO() {
    return make(static_cast<long>(0), static_cast<long>(0));
}

TypeInteger *TypeInteger::U1() {
    return make(static_cast<long>(0), static_cast<long>(1));
}

TypeInteger *TypeInteger::BOOL() {
    return U1();
}

TypeInteger *TypeInteger::FALSE() {
    return ZERO();
}

TypeInteger *TypeInteger::TRUE() {
    return make(static_cast<long>(1), static_cast<long>(1));
}

Type* TypeInteger::glb() {
    return BOT();
}

TypeInteger *TypeInteger::I8() {
    return make(static_cast<long>(-128), static_cast<long>(127));
}

TypeInteger *TypeInteger::I16() {
    return make(static_cast<long>(-32768),static_cast<long> (32767));
}

TypeInteger *TypeInteger::I32() {
    return make(static_cast<long>(static_cast<unsigned long>(-1) << 31),
                static_cast<long>((1UL << 31) - 1));
}

TypeInteger *TypeInteger::U8() {
    return make(static_cast<long>(0), static_cast<long>(255));
}

TypeInteger *TypeInteger::U16() {
    return make(static_cast<long>(0), static_cast<long>(65535));
}

TypeInteger *TypeInteger::U32() {
    return make(static_cast<long>(0), static_cast<long>((1ULL << 32) - 1));
}

TypeInteger *TypeInteger::makeInit() {
    return ZERO();
}

std::string TypeInteger::str() {
    if (this == TOP()) return "~int";
    if (this == BOT()) return "_int";
    if(this == BOOL()) return "bool";
    if(this == I8()) return "i8";
    if(this == I16()) return "i16";
    if(this == I32()) return "i32";
    if(this == U8()) return "u8";
    if(this == U16()) return "u16";
    if(this == U32()) return "u32";
    if(isConstant()) return std::to_string(min_);
    return "[" + std::to_string(min_) + "-" + std::to_string(max_) + "]";
}

Type *TypeInteger::xmeet(Type *other) {
    // Invariant from caller: 'this' != 'other' and same class (TypeInteger)
    auto *i = dynamic_cast<TypeInteger *>(other);
//    if (this == BOT())
//        return this;
//
//    if (i == BOT())
//        return i;
//    // TOP loses
//    if (i == TOP())
//        return this;
//    if (this == TOP())
//        return i;
//
//    // Since both are constants, and are never equals (contract) unequals
//    // constants fall to bottom
//    return BOT();
return make(std::min(min_, i->min_), std::max(max_, i->max_));
}

Type *TypeInteger::dual() {
    return make(max_, min_);
}

TypeInteger *TypeInteger::make(bool is_con, long con) {
    return make(static_cast<long>(is_con ? con : (con == 0 ? std::numeric_limits<long>::max() : std::numeric_limits<long>::min())),
            static_cast<long>(is_con ? con : (con == 0 ? std::numeric_limits<long>::min() : std::numeric_limits<long>::max())));

}

TypeInteger *TypeInteger::make(long lo, long hi) {
    auto*obj = dynamic_cast<TypeInteger*>((alloc.new_object<TypeInteger>(lo, hi))->intern());
    return obj;
}

long TypeInteger::value() { assert(isConstant()); return min_; }

int TypeInteger::hash() { return (min_ ^ (min_ >> 32)) * (max_ ^ (max_ >> 32)); }

long TypeInteger::mask() {
    if(isHigh()) return 0;
    if(isConstant()) return min_;
    //if( _min<0 ) return -1L;
    //if( _max==Long.MAX_VALUE ) return -1L;
    // Those bit positions which differ min to max
    std::uint64_t x = min_ ^ max_;

    // Highest '1' bit in the differ set.  Since the range is from min to
    // max, all values below here are possible.
    std::uint64_t ff1 = std::bit_floor(x);
    // Make a all-1's mask from ff1, and set over the same bits (either min
    // or max is ok).
    long mask = min_ | (ff1-1) | ff1;
    return mask;
}
bool TypeInteger::eq(Type *t) {
    auto *i = dynamic_cast<TypeInteger *>(t);
    return min_ == i->min_ && max_ == i->max_;
}