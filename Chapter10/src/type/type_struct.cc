#include "../../Include/type/type_struct.h"

TypeStruct::TypeStruct(std::string name, Tomi::Vector<Field *> fields) : Type(TSTRUCT), name_(name), fields_(fields) {

}

TypeStruct *TypeStruct::make(std::string name, Tomi::Vector<Field *> fields) {
    return dynamic_cast<TypeStruct *>((new TypeStruct(name, fields))->intern());
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
    for (int i = 0; i < fields_.size(); i++) {
        if (fields_[i]->fname_ == fname) {
            return i;
        }
    }
    return -1;
}

Type *TypeStruct::xmeet(Type *t) {
    TypeStruct *that = dynamic_cast<TypeStruct *>(t);
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
    // Now all fields should be the same, so just do field meets
    assert(fields_.size() == that->fields_.size());
    Tomi::Vector<Field *> newFields(fields_.size());
    for (int i = 0; i < fields_.size(); i++) {
        newFields[i] = fields_[i]->xmeet(that->fields_[i]);
    }
    return make(name_, newFields);
}

TypeStruct *TypeStruct::dual() {
    if (this == TOP()) return BOT();
    if (this == BOT()) return TOP();
    Tomi::Vector<Field *> newFields(fields_.size());
    for (int i = 0; i < fields_.size(); i++) {
        newFields[i] = fields_[i]->dual();
    }
    return make(name_, newFields);
}

TypeStruct *TypeStruct::glb() {
    if (glb_()) return this;
    Tomi::Vector<Field *> newFields(fields_.size());
    for (int i = 0; i < fields_.size(); i++) {
        newFields[i] = fields_[i]->glb();
    }
    return make(name_, newFields);
}

bool TypeStruct::glb_() {
    for (Field *f: fields_) {
        if (f->glb() != f) return false;

    }
    return true;
}

bool TypeStruct::eq(Type *t) {
    TypeStruct *that = dynamic_cast<TypeStruct *>(t);
    if (name_ != that->name_) return false;
    if (fields_.size() != that->fields_.size()) return false;
    for (int i = 0; i < fields_.size(); i++) {
        if (fields_[i] != that->fields_[i]) return false;
    }
    return true;
}

int TypeStruct::hash() {
    long hash = std::hash < std::string > {}(name_);
    for (Field *f: fields_) {
        hash ^= f->hash();
    }
    return hash;
}

std::ostringstream &TypeStruct::print_1(std::ostringstream &builder) {
    builder << name_ << "{";
    for (int i = 0; i < fields_.size(); i++) {
        fields_[i]->print_1(builder);
        if (i < fields_.size() - 1) {
            builder << ",";
        }
    }
    builder << "}";
    return builder;
}

std::string TypeStruct::str() {
    return name_;
}