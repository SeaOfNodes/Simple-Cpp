#include "../../Include/type/field.h"
#include "../../Include/type/integer_type.h"

Field::Field(std::string fname, int alias, Type *type) : Type(TFLD), fname_(fname), alias_(alias), type_(type) {}

Field *Field::make(std::string fname, int alias, Type *type) {
    return dynamic_cast<Field *>((alloc.new_object<Field>(fname, alias, type))->intern());
}

Field *Field::test() {
    static Field *TEST = make("TEST", -2, TypeInteger::ZERO());
    return TEST;
}

void Field::gather(Tomi::Vector<Type *> &ts) {
    ts.push_back(test());
}

Field *Field::xmeet(Type *that) {
    Field *fld = dynamic_cast<Field *>(that);
    // Todo: Missing assert(how is it true?)
    assert(fname_ == fld->fname_ && alias_ == fld->alias_);
    return make(fname_, alias_, type_->meet(fld->type_));
}

Field *Field::dual() {
    return make(fname_, alias_, type_->dual());
}

Field *Field::glb() {
    return make(fname_, alias_, type_->glb());
}

int Field::hash() {
    return std::hash<std::string >{}(fname_) ^ type_->hash() ^ alias_;
}

bool Field::eq(Type *t) {
    auto *fld = dynamic_cast<Field *>(t);
    return fname_ == fld->fname_ && type_ == fld->type_ && alias_ == fld->alias_;
}

std::ostringstream &Field::print_1(std::ostringstream &builder) {
    builder << fname_ << ":";
    builder << alias_ << ":";
    return builder;
}

std::string Field::str() {
    return fname_;
}