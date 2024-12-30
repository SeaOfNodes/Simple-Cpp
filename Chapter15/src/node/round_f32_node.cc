#include "../Include/node/roundf32_node.h"
#include "../Include/type/type_float.h"

RoundF32Node::RoundF32Node(Node *lhs) : Node{nullptr, lhs} {}

std::string RoundF32Node::label() { return "RoundF32"; }

std::string RoundF32Node::glabel() { return "roundf32"; }

std::ostringstream &RoundF32Node::print_1(std::ostringstream &builder, Tomi::Vector<bool> &visited) {
    builder << "((f32)";
    in(1)->print_0(builder, visited);
    builder << ")";
    return builder;
}

Type* RoundF32Node::compute() {
    auto* i0 = dynamic_cast<TypeFloat*>(in(1)->type_);
    if(i0 && i0->isConstant()) {
        return TypeFloat::constant(static_cast<float>(i0->value()));
    }
    return in(1)->type_;
}

Node* RoundF32Node::idealize() {
    Node*lhs = in(1);
    Type*t1 = lhs->type_;

    // roundf32 of a constant
    auto*tf = dynamic_cast<TypeFloat*>(t1);
    if(tf && tf->sz_ == 32) {
        return lhs;
    }
    return nullptr;
}