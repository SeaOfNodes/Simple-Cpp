#ifndef TYPE_STRUCT_H
#define TYPE_STRUCT_H
#include "../type/type.h"
#include "../type/field.h"
#include <cassert>

class TypeStruct: public Type {
public:
    std::string name_;
    Tomi::Vector<Field*> fields_;
    TypeStruct(std::string name, Tomi::Vector<Field*> fields);
    static TypeStruct* make(std::string name, Tomi::Vector<Field*> fields);
    static TypeStruct* TOP();
    static TypeStruct* BOT();
    static TypeStruct* test();
    static void gather(Tomi::Vector<Type*>& ts);

    int find(std::string fname);
    Type* xmeet(Type* that) override;

    TypeStruct* dual();
    TypeStruct* glb();
    bool glb_();

    bool eq(Type* t) override;

    int hash()  override;
    std::ostringstream &print_1(std::ostringstream &builder) override;
    std::string str() override;
};

#endif