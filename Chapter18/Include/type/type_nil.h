#ifndef TYPE_NIL_H
#define TYPE_NIL_H

#include "type.h"

class TypeNil : public Type {
public:
    // 0 = high-subclass choice nil
    // 1 = high-subclass no nil
    // 2 = low -subclass no nil
    // 3 = low -subclass also nil
    unsigned int nil_{};
    TypeNil(unsigned int t, unsigned int nil);
    static void gather(Tomi::Vector<Type *> &ts);

    virtual TypeNil* make_from(unsigned int nil);

    unsigned int xmeet0(TypeNil* that);
    unsigned int dual0();

    // RHS is NIL
    virtual Type* meet0();
    // RHS is XNIL
    Type* meetX();

    Type* nmeet();

    bool isHigh() override;
    bool isConstant() override;
    bool isHighOrConst() override;

    Type* glb() override;

    bool notNull();
    bool nullable();

    std::string q();
    std::string x();

    int hash();
    bool eq(TypeNil* ptr);

};
#endif