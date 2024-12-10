#ifndef TYPE_MEM_PTR_H
#define TYPE_MEM_PTR_H
#include "type.h"
#include "type_struct.h"

/**
 * Represents a Pointer to memory.
 *
 * Null is generic pointer to non-existent memory.
 * *void is a non-Null pointer to all possible structs.
 * Pointers can be to specific struct types, or a union with Null.
 * The distinguished *$BOT ptr represents union of *void and Null.
 * The distinguished *$TOP ptr represents the dual of *$BOT.
 */
class TypeMemPtr: public Type {
    // A TypeMemPtr is pair (obj,nil)
    // where obj is one of
    //    (null,TypeStruct).
    // where nil is one of
    //    (true,false) meaning an explicit null is allowed or not
public:
    TypeStruct* obj_;  // null, a TypeStruct, or sentinel TypeStruct.MANY
    bool nil_;

    TypeMemPtr(TypeStruct* obj, bool nil);
    static TypeMemPtr* make(TypeStruct* obj, bool nil);
    static TypeMemPtr* make(TypeStruct* obj);

    static TypeMemPtr* BOT();
    static TypeMemPtr* TOP();
    static TypeMemPtr* NULLPTR();
    static TypeMemPtr* VOIDPTR();
    static TypeMemPtr* TEST();

    static void gather(Tomi::Vector<Type*>& ts);
    Type* xmeet(Type* other) override;
    TypeMemPtr* dual();
    TypeMemPtr* glb();

    TypeMemPtr* makeInit() override;

    int hash();

    bool eq(Type* other) override;
    std::ostringstream &print_1(std::ostringstream &builder) override;
    std::string str();
};

#endif