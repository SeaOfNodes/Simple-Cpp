#include "../../Include/type/type_struct.h"
#include "../../Include/type/field.h"
#include "../../Include/type/integer_type.h"
#include "../../Include/type/type_mem_ptr.h"
#include "../../Include/type/type_float.h"

TypeStruct::TypeStruct(std::string name, std::optional<Tomi::Vector<Field *>> fields) : Type(TSTRUCT), name_(name), fields_(fields) {
}

TypeStruct *TypeStruct::make(std::string name, Tomi::Vector<Field *> fields) {
    return dynamic_cast<TypeStruct *>((alloc.new_object<TypeStruct>(name, fields))->intern());
}

TypeStruct* TypeStruct::make(std::string name) {

    auto*a = dynamic_cast<TypeStruct*>((alloc.new_object<TypeStruct>(name, std::nullopt))->intern());
    return a;
}

TypeStruct* TypeStruct::make_Ary(TypeInteger* len, int lenAlias, Type *body, int bodyAlias) {
    Tomi::Vector<Field*> fields;
    fields.push_back(Field::make("#", len, lenAlias, true));
    fields.push_back(Field::make("[]", body, bodyAlias, true));
    return make("[" + body->str() + "]", fields);

}
TypeStruct* TypeStruct::S1F() {
    TypeStruct* s1f = make("S1");
    return s1f;
}
TypeStruct* TypeStruct::ARY() {
    // what does this represent
    return make_Ary(TypeInteger::BOT(), -1, TypeFloat::BOT(),  -2);
}
TypeStruct* TypeStruct::S2F() {
    TypeStruct* s2f = make("S2");
    return s2f;
}
TypeStruct* TypeStruct::S1() {
    Tomi::Vector<Field*> fields;
    fields.push_back(Field::make("a", TypeInteger::BOT(), -1, false));
    fields.push_back(Field::make("s2", TypeMemPtr::make(S2F(), false), -2,  false));
    TypeStruct* s1 = TypeStruct::make("S1", fields);
    return s1;
}

TypeStruct* TypeStruct::S2() {
    Tomi::Vector<Field*> fields;
    fields.push_back(Field::make("S1", TypeMemPtr::make(S1F(), false), -3, false));
    fields.push_back(Field::make("b", TypeFloat::BOT(), -4, false));

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
    ts.push_back(S1());
    ts.push_back(S2());
    ts.push_back(ARY());
}

// Find field index by name
int TypeStruct::find(std::string fname) {
    for (int i = 0; i < fields_.value().size(); i++) {
        if (fields_.value()[i]->fname_ == fname) {
            return i;
        }
    }
    return -1;
}

int TypeStruct::findAlias(int alias) {
    for(int i = 0; i < fields_.value().size(); i++) {
        if(fields_.value()[i]->alias_ == alias) {
            return i;
        }
    }
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
    if(fields_.value().size() != that->fields_.value().size()) return BOT();

    // Now all fields should be the same, so just do field meets
    assert(fields_.value().size() == that->fields_.value().size());
    Tomi::Vector<Field *> newFields(fields_.value().size());
    for (int i = 0; i < fields_.value().size(); i++) {
        Field*f0 = fields_.value()[i];
        Field*f1 = that->fields_.value()[i];
        if(f0->fname_ != f1->fname_ || f0->alias_ != f1->alias_) return BOT();

        // guaranteed that the fields have the same name and alias.
        newFields[i] = dynamic_cast<Field*>(f0->meet(f1));
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
    if(fields_.has_value()) {
        for (Field *f: fields_.value()) {
            if (f->glb() != f) return false;

        }
    }
    return true;
}

bool TypeStruct::isAry() {
    return fields_.value().size() == 2 && fields_.value()[1]->fname_ == "[]";
}
int TypeStruct::aryBase() {
    assert(isAry());
    // cache offsets here
    if(offs_.empty()) {
        offs_ = offsets();
    }
    return offs_[1];
}
int TypeStruct::aryScale() {
    assert(isAry());
    return fields_.value()[1]->type_->log_size();
}
int TypeStruct::offset(int idx) {
    if(offs_.empty()) {
        offs_ = offsets();

    }
    return offs_[idx];
}
// Field offsets as packed byte offsets
Tomi::Vector<int> TypeStruct::offsets() {
    // Compute a layout for a collection of fields
    assert(fields_.has_value());
    // Compute a layout
    int cnts[4] = {0};
    for(Field*f : fields_.value()) {
        cnts[f->type_->log_size()]++;  // Log size is 0(byte), 1(i16/u16), 2(i32/f32), 3(i64/dbl)
    }
    int off = 0;
    int idx = 0;
    Tomi::Vector<int> offs(4);
    for(int i = 3; i >= 0; i--) {
        offs[i] = off;
        off += cnts[i] << i;
    }
    // Assign offsets to all fields.
    // Really a hidden radix sort.

    offs_.resize(fields_.value().size() + 1);
    for(Field* f : fields_.value()) {
        int log = f->type_->log_size();
        offs_[idx++] = offs[log];  // Field offset
        offs[log] += 1 << log;   // Next field offset at same alignment
        cnts[log]--;    // Count down, should be all zero at end
    }
    offs_[fields_.value().size()] = (off+7)& ~7;
    return offs_;
}

bool TypeStruct::eq(Type *t) {
    TypeStruct *that = dynamic_cast<TypeStruct *>(t);
    if (name_ != that->name_) return false;
    if(!fields_.has_value()) {
        std::cerr << "st";
    }
    if(fields_ == that->fields_) return true;
    if(!fields_.has_value() || !that->fields_.has_value()) return false;
    if (fields_.value().size() != that->fields_.value().size()) return false;
    for (int i = 0; i < fields_.value().size(); i++) {
        if (fields_.value()[i] != that->fields_.value()[i]) return false;
    }
    return true;
}

int TypeStruct::hash() {
    long hash = std::hash < std::string > {}(name_);
    if(fields_.has_value()) {
        for (Field *f: fields_.value()) {
            hash ^= f->hash();
        }
    }
    // fold long into int (64 bits into 32 bits);
    return (int) (hash ^ (hash >> 32));
}

std::ostringstream &TypeStruct::print_1(std::ostringstream &builder) {
    // Forward reference struct, just print the name
    if(!fields_) return builder;
    builder << name_ << " {";
    for (int i = 0; i < fields_.value().size(); i++) {
        fields_.value()[i]->print_1(builder);
        if (i < fields_.value().size() - 1) {
            builder << ", ";
        }
    }
    builder << "}";
    return builder;
}

std::string TypeStruct::str() {
    return name_;
}