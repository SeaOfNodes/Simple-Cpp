#include "../../Include/type/field.h"
#include "../../Include/type/integer_type.h"

Field::Field(std::string fname, Type *type) : Type(TFLD), fname_(fname), type_(type) {}

Field *Field::make(std::string fname, Type *type) {
    return dynamic_cast<Field *>((alloc.new_object<Field>(fname, type))->intern());
}

Field *Field::test() {
    static Field *TEST = make("TEST", TypeInteger::ZERO());
    return TEST;
}

void Field::gather(Tomi::Vector<Type *> &ts) {
    ts.push_back(test());
}

Field *Field::xmeet(Type *that) {
    Field *fld = dynamic_cast<Field *>(that);
    // Todo: Missing assert
    return make(fname_, type_->meet(fld->type_));
}

Field *Field::dual() {
    return make(fname_, type_->dual());
}

Field *Field::glb() {
    return make(fname_, type_->glb());
}

int Field::hash() {
    return std::hash<std::string >{}(fname_) ^ type_->hash();
}

bool Field::eq(Type *t) {
    auto *fld = dynamic_cast<Field *>(t);
    return fname_ == fld->fname_ && type_ == fld->type_;
}

std::ostringstream &Field::print_1(std::ostringstream &builder) {
    builder << fname_ << ":";
    type_->print_1(builder);
    return builder;
}

std::string Field::str() {
    return fname_;
}