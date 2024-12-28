#ifndef TYPE_FLOAT_H
#define TYPE_FLOAT_H

#include "type.h"
#include <cstddef>

class TypeFloat;

template<> struct Tomi::hash<TypeFloat*> {
    unsigned long long operator()(TypeFloat* val);
};
class TypeFloat: public Type {
public:
    static TypeFloat* TOP();
    static TypeFloat* BOT();
    static TypeFloat* ZERO();
    static TypeFloat* T32();
    static TypeFloat* B32();

    /** +/-64 for double; +/-32 for float, or 0 for constants */
    std::int8_t sz_;

    bool is_con_{};
    /**
    * The constant value or
    * if not constant then 1=bottom, 0=top.
    */
    double con_;
    TypeFloat(std::int8_t sz, double con);
    static TypeFloat* make(std::int8_t sz, double con);

    static TypeFloat* constant(double con);
    static void gather(Tomi::Vector<Type *> &ts);
    std::ostringstream &print_1(std::ostringstream &builder) override;
    std::string str() override;

    std::ostringstream& typeName(std::ostringstream& builder) override;
    bool isHighOrConst() override;
    bool isF32();
    bool isHigh();
    bool isConstant() override;
    double value();

    Type* xmeet(Type *other) override;
    Type* dual() override;
    Type* glb() override;
    TypeFloat* makeInit() override;
    int hash() override;

    bool eq(Type *other) override;
};
#endif