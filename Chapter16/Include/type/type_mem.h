#ifndef TYPE_MEM_H
#define TYPE_MEM_H
/**
 * Represents a slice of memory corresponding to a set of aliases
 */

#include "../../Include/type/type.h"

class TypeMem: public Type {
    // Which slice of memory?
    //  0 means TOP, no slice.
    // -1 means BOT, all memory.
    //  N means slice#N.
public:
    int alias_;
    Type* t_ = nullptr;    // Memory contents, some scalar type
    explicit TypeMem(int alias, Type*t);
    static TypeMem* make(int alias, Type*t);
    static TypeMem* TOP();
    static TypeMem* BOT();

    void gather(Tomi::Vector<Type*>& ts);
    TypeMem* xmeet(Type* t);
    Type* dual() override;
    Type* glb();
    int hash() override;
    bool eq(Type* t) override;
    std::ostringstream &print_1(std::ostringstream &builder) override;

    std::string str() override;
};

#endif