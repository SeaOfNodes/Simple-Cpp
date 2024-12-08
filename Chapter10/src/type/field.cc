#include "../../Include/type/field.h"
#include "../../Include/type/integer_type.h"

Field::Field(std::string fname, Type* type) : Type(TFLD), fname_(fname), type_(type) {}
Field* Field::make(std::string fname, Type *type) {
    return static_cast<Field*>((new Field(fname, type))->intern());
}
Field* Field::TEST = new Field("TEST", TypeInteger::ZERO());
void Field::gather(Tomi::Vector<Type*> ts) {
        ts.push_back(TEST);
}
Field* Field::xmeet(Type* that) {
    Field* fld = dynamic_cast<Field*>(that);
    // Todo: Missing assert
    return make(fname_, type_->meet(fld->type_));
}
Field* Field::dual() {
    return make(fname_, type_->dual());
}
Field* Field::glb() {
    return make(fname_, type_->glb());
}

int Field::hash() {
    return std::hash<std::string>{}(fname_) ^ type_->hash();
}

bool Field::eq(Type* t) {
    Field* fld = dynamic_cast<Field*>(t);
    return fname_ == fld->fname_ && type_->eq(fld->type_);
}

std::ostringstream &Field::print_1(std::ostringstream &builder) {
    builder << fname_ << ":";
    type_->print_1(builder);
    return builder;
}

std::string Field::str() {
    return fname_;
}