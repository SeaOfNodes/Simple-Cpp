#ifndef FIELD_H
#define FIELD_H

#include "../type/type.h"
#include <functional>

class Field : public Type {
public:
    // The pair {fieldName,type} uniquely identifies a field.
    // Field name
    std::string fname_;
    // Type of the field
    Type *type_;

    Field(std::string fname, Type *type);

    static Field *make(std::string fname, Type *type);

    static Field *TEST();


    static void gather(Tomi::Vector<Type *> &ts);

    Field *xmeet(Type *that);

    Field *dual();

    Field *glb();

    int hash() override;

    bool eq(Type *t) override;

    std::string str() override;

    std::ostringstream &print_1(std::ostringstream &builder) override;
};

#endif