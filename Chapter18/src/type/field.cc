#include "../../Include/type/field.h"
#include "../../Include/type/integer_type.h"

Field::Field(std::string fname,  Type *type, int alias,bool xfinal) : Type(TFLD), fname_(fname), alias_(alias), type_(type), final_(xfinal) {}

Field *Field::make(std::string fname, Type *type, int alias,  bool xfinal) {
    return dynamic_cast<Field *>((alloc.new_object<Field>(fname, type, alias, xfinal))->intern());
}

Field *Field::test() {
    static Field *TEST = make("TEST", TypeInteger::ZERO(), -2 , false);
    return TEST;
}

Field* Field::test2() {
    static Field* TEST2 = make("TEST2", TypeInteger::ZERO(), -3, false);
    return TEST2;
}
void Field::gather(Tomi::Vector<Type *> &ts) {
    ts.push_back(test());
}

Field *Field::xmeet(Type *that) {
    Field *fld = dynamic_cast<Field *>(that);
    // Todo: Missing assert(how is it true?)
    assert(fname_ == fld->fname_ && alias_ == fld->alias_ && final_ == fld->final_);
    return make(fname_, type_->meet(fld->type_), alias_, final_ | fld->final_);
}

Field *Field::dual() {
    return make(fname_, type_->dual(), alias_, !final_);
}

Field *Field::makeFrom(Type *type) {
    return type == type_ ? this:  dynamic_cast<Field*>((alloc.new_object<Field>(fname_, type, alias_, final_))->intern());
}
Field *Field::glb() {
    Type*glb = type_->glb();
    return glb  == type_ && final_ ? this : make(fname_, glb, alias_, final_);
}

Field* Field::makeR0() {
    return final_ ? this : make(fname_, type_->makeR0(), alias_, true);
}
Field* Field::lub() {
    Type*lub = type_->lub();
    return lub == type_ ? this : make(fname_, lub, alias_, final_);
}
bool Field::isFinal() {
    return final_ && type_->isFinal();
}

int Field::hash() {
    return std::hash<std::string >{}(fname_) ^ type_->hash() ^ alias_ ^ (final_ ? 1024 : 0);
}

bool Field::eq(Type *t) {
    auto *fld = dynamic_cast<Field *>(t);
    return fname_ == fld->fname_ && type_ == fld->type_ && alias_ == fld->alias_ && final_ == fld->final_;
}

std::ostringstream &Field::print_1(std::ostringstream &builder) {
    builder << (final_ ? "" : "!");
    builder << fname_ << ":";
    builder << alias_ << ":";
    return builder;
}

std::string Field::str() {
    return fname_;
}