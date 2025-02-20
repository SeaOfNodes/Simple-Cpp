#include "../../Include/node/to_float_node.h"
#include "../../Include/type/type_float.h"

ToFloatNode::ToFloatNode(Node *lhs) : Node({nullptr, lhs}) {}

std::string ToFloatNode::label() {
    return "ToFloat";
}

std::string ToFloatNode::glabel() {
    return "(flt)";
}

std::ostringstream &ToFloatNode::print_1(std::ostringstream &builder, Tomi::Vector<bool> &visited) {
    builder << "(flt)";
    in(1)->print(builder);
    return builder;
}

Type *ToFloatNode::compute() {
    if(in(1)->type_ == Type::NIL()) return TypeFloat::FZERO();
    auto *t1 = dynamic_cast<TypeInteger *>(in(1)->type_);
    if (t1) {
        if(t1->isHigh()) return TypeFloat::F64()->dual();
        if (t1->isConstant()) {
            return TypeFloat::constant(static_cast<double>(t1->value()));
        }
    }
    return TypeFloat::F64();
}

Node *ToFloatNode::idealize() {
    return nullptr;
}

Node *ToFloatNode::copy(Node *lhs) {
    return alloc.new_object<ToFloatNode>(lhs);
}