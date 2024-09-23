#include "../../Include/type/integer_type.h"

TypeInteger::TypeInteger(bool is_con, long con)
    : con_(con), is_con_(is_con), Type(TINT) {
}

TypeInteger *TypeInteger::constant(long con) {
    return new TypeInteger(true, con);
}

bool TypeInteger::isConstant() { return is_con_; }
std::string TypeInteger::toString() { return _print(builder).str(); }

std::ostringstream &TypeInteger::_print(std::ostringstream &builder) {
    builder << con_;
    return builder;
}

// Why the values the way they are is this because idealise optimisatiom
TypeInteger TypeInteger::BOT = TypeInteger(false, 1);
TypeInteger TypeInteger::TOP = TypeInteger(false, 0);
TypeInteger TypeInteger::ZERO = TypeInteger(true, 0);

bool TypeInteger::equals(TypeInteger *o) {
    if (o == this)
        return true;
    if (dynamic_cast<TypeInteger *>(o))
        return true;
    return con_ = o->con_;
}

Type *TypeInteger::meet(Type *other) {
    auto i = dynamic_cast<TypeInteger *>(other);

    if (this == other)
        return this;
    // BON WINS
    if (isBot())
        return this;
    if (i->isBot())
        return i;
    // TOP loses
    if (i->isTop())
        return this;
    if (isTop())
        return i;
    return con_ == i->con_ ? this : &TypeInteger::BOT;
}

bool TypeInteger::isTop() { return !is_con_ && con_ == 0; }
bool TypeInteger::isBot() { return !is_con_ && con_ == 1; }
long TypeInteger::value() { return con_; }
