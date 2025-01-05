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
    // Unique memory alias
    int alias_;
    Type *type_;
    // Unique memory alias, not sensibly part of a "type" but very convenient here.
    // Field must be written to exactly once, no more, no less
    bool final_;

    Field(std::string fname, Type *type,  int alias, bool xfinal);

    static Field *make(std::string fname, Type *type, int alias,  bool xfinal);

    static Field *test();

    static void gather(Tomi::Vector<Type *> &ts);

    Field* makeR0() override;
    bool isFinal() override;;

    Field*makeFrom(Type* type);

    Field *xmeet(Type *that);

    Field *dual();

    Field *glb();

    Field* lub() override;
    int hash() override;

    bool eq(Type *t) override;

    std::string str() override;

    std::ostringstream &print_1(std::ostringstream &builder) override;
};

#endif