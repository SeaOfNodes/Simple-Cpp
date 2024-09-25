#include "../../Include/type/tuple_type.h"

TypeTuple::TypeTuple(std::initializer_list<Type *> types) : Type(TTUPLE) {
    for (auto type: types) {
        types_.push_back(type);
    }
}

Type *TypeTuple::meet() {
    throw std::runtime_error("Meet on Tuple type not yet implemented!");
}

std::ostringstream TypeTuple::print_1(std::ostringstream &builder) {
    builder << "[";
    // fence-post problem here
    for (auto t: types_) {
        t->_print(builder);
        builder << ",";
    }
    builder << "]";
}
