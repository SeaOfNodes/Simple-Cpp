#include "../../Include/type/type_struct.h"
#include "../../Include/type/field.h"
#include "../../Include/type/integer_type.h"
#include "../../Include/type/type_mem_ptr.h"
#include "../../Include/type/type_float.h"

TypeStruct::TypeStruct(std::string name, Tomi::Vector<Field *> fields) : Type(TSTRUCT), name_(name), fields_(fields) {

}

TypeStruct *TypeStruct::make(std::string name, Tomi::Vector<Field *> fields) {
    return dynamic_cast<TypeStruct *>((alloc.new_object<TypeStruct>(name, fields))->intern());
}

TypeStruct* TypeStruct::make(std::string name) {
    return dynamic_cast<TypeStruct*>((alloc.new_object<TypeStruct>(name, Tomi::Vector<Field*>{})->intern()));
}

TypeStruct* TypeStruct::S1F() {
    TypeStruct* s1f = make("S1");
    return s1f;
}
TypeStruct* TypeStruct::S2F() {
    TypeStruct* s2f = make("S2");
    return s2f;
}
TypeStruct* TypeStruct::S1() {
    Tomi::Vector<Field*> fields;
    fields.push_back(Field::make("a", TypeInteger::BOT()));
    fields.push_back(Field::make("s2", TypeMemPtr::make(S2F(), false)));
    TypeStruct* s1 = TypeStruct::make("S1", fields);
    return s1;
}

TypeStruct* TypeStruct::S2() {
    Tomi::Vector<Field*> fields;
    fields.push_back(Field::make("S1", TypeMemPtr::make(S1F(), false)));
    fields.push_back(Field::make("b", TypeFloat::BOT()));

    TypeStruct* s2 = TypeStruct::make("S2", fields);
    return s2;
}

TypeStruct *TypeStruct::TOP() {
    TypeStruct *top = make("$TOP", Tomi::Vector<Field *>());
    return top;
}

TypeStruct *TypeStruct::BOT() {
    TypeStruct *bot = make("$BOT", Tomi::Vector<Field *>());
    return bot;

}

TypeStruct *TypeStruct::test() {
    TypeStruct *test = make("$BOT", Tomi::Vector<Field *>{Field::test()});
    return test;
}

void TypeStruct::gather(Tomi::Vector<Type *> &ts) {
    ts.push_back(test());
    ts.push_back(BOT());
}

int TypeStruct::find(std::string fname) {
    for (int i = 0; i < fields_.value().size(); i++) {
        if (fields_.value()[i]->fname_ == fname) {
            return i;
        }
    }
    return -1;
}

Type *TypeStruct::xmeet(Type *t) {
    auto *that = dynamic_cast<TypeStruct *>(t);
    if (this == TOP()) return that;
    if (that == TOP()) return this;
    if (this == BOT()) return BOT();
    if (that == BOT()) return BOT();

    // Within the same compilation unit, struct names are unique.  If the
    // names differ, its different structs.  Across many compilation units,
    // structs with the same name but different field layouts can be
    // interned... which begs the question:
    // "What is the meet of structs from two different compilation units?"
    // And the answer is: "don't ask".

    if (name_ != that->name_) {
        return BOT();  // It's a struct; that's about all we know
    }
    if(!fields_.has_value()) return this;
    if(!that->fields_.has_value()) return that;
    // Now all fields should be the same, so just do field meets
    if(fields_.value().size() != that->fields_.value().size()) {
        std::cerr << "st";
    }
    assert(fields_.value().size() == that->fields_.value().size());
    Tomi::Vector<Field *> newFields(fields_.value().size());
    for (int i = 0; i < fields_.value().size(); i++) {
        newFields[i] = fields_.value()[i]->xmeet(that->fields_.value()[i]);
    }
    return make(name_, newFields);
}

TypeStruct *TypeStruct::dual() {
    if (this == TOP()) return BOT();
    if (this == BOT()) return TOP();
    if(!fields_) return this;

    Tomi::Vector<Field *> newFields(fields_.value().size());
    for (int i = 0; i < fields_.value().size(); i++) {
        newFields[i] = fields_.value()[i]->dual();
    }
    return make(name_, newFields);
}

TypeStruct *TypeStruct::glb() {
    if (glb_()) return this;
    Tomi::Vector<Field *> newFields(fields_.value().size());
    for (int i = 0; i < fields_.value().size(); i++) {
        newFields[i] = fields_.value()[i]->glb();
    }
    return make(name_, newFields);
}

bool TypeStruct::glb_() {
    if(!fields_) {
        for (Field *f: fields_.value()) {
            if (f->glb() != f) return false;

        }
    }
    return true;
}

bool TypeStruct::eq(Type *t) {
    TypeStruct *that = dynamic_cast<TypeStruct *>(t);
    if (name_ != that->name_) return false;
    if(fields_ == that->fields_) return true;
    if (fields_.value().size() != that->fields_.value().size()) return false;
    for (int i = 0; i < fields_.value().size(); i++) {
        if (fields_.value()[i] != that->fields_.value()[i]) return false;
    }
    return true;
}

int TypeStruct::hash() {
    long hash = std::hash < std::string > {}(name_);
    if(!fields_) {
        for (Field *f: fields_.value()) {
            hash ^= f->hash();
        }
    }
    // fold long into int (64 bits into 32 bits);
    return (int) (hash ^ (hash >> 32));
}

std::ostringstream &TypeStruct::print_1(std::ostringstream &builder) {
    builder << name_ << "{";
    // Forward reference struct, just print the name
    if(!fields_) return builder;
    for (int i = 0; i < fields_.value().size(); i++) {
        fields_.value()[i]->print_1(builder);
        if (i < fields_.value().size() - 1) {
            builder << ",";
        }
    }
    builder << "}";
    return builder;
}

std::string TypeStruct::str() {
    return name_;
}