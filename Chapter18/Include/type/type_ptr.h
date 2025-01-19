#ifndef TYPE_PTR_H
#define TYPE_PTR_H

#include "type.h"
#include "type_nil.h"
class TypePtr : public TypeNil {
    TypePtr(unsigned int nil);
    // An abstract pointer, pointing to either a Struct or an Array.
    // Can also be null or not, so 4 choices {TOP,BOT} x {nil,not}
    static TypePtr* XPTR();
    static TypePtr* XNPTR();
    static TypePtr* NPTR();
    static TypePtr* PTR();
    const static TypePtr* PTRS[4] = {XPTR(), XNPTR(), NPTR(), PTR()};
    static void gather(Tomi::Vector<Type*>& ts);
    TypeNil* makeFrom(unsigned int nil);
    Type*xmeet(Type*t t) override;
    TypePtr* dual() override;
    // High scalar loses, low scalar wins
    TypeNil* nmeet(TypeNil* tn) override;
    // RHS is  NIL
    Type* meet0() override;
    // RHS is XNIL
    // 0->xscalar, 1->nscalar, 2->nscalar, 3->scalar
    Type* meetX() override;
    TypePtr*glb() override;
    const static char* STRS[4] = {"~ptr", "~nptr", "nptr", "ptr"};
    std::string str() override;
    std::ostringstream& print(std::ostringstream& builder) override;
}

#endif