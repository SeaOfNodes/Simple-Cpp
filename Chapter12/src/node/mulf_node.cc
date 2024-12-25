#include "../../Include/node/mulf_node.h"

MulfNode::MulfNode(Node* lhs, Node* rhs) : Node({nullptr, lhs, rhs}) {}

std::string MulfNode::label() {
    return "MulF";
}
std::string MulfNode::glabel() {
    return "*";
}

std::ostringstream &MulfNode::print_1(std::ostringstream &builder, Tomi::Vector<bool>& visited) {
    builder << "(";
    in(1)->print_0(builder, visited);

    builder << "*";
    in(2)->print_0(builder, visited);

    builder << ")";

    return builder;
}

Type *MulfNode::compute() {
    auto i0 = dynamic_cast<TypeFloat *>(in(1)->type_);
    auto i1 = dynamic_cast<TypeFloat *>(in(2)->type_);

    if (i0 && i1) {
        if (i0->isConstant() && i1->isConstant()) {
            return TypeFloat::constant(i0->value() * i1->value());
        }
    }

    return in(1)->type_->meet(in(2)->type_);
}

Node *MulfNode::idealize() {
    Node *lhs = in(1);
    Node *rhs = in(2);

    Type *t1 = lhs->type_;
    Type *t2 = rhs->type_;

    auto *i = dynamic_cast<TypeFloat *>(t2);

    // Mul of 1
    if (t2->isConstant() && i && i->value() == 1) return lhs;

    // Move constants to RHS: con*arg becomes arg*con

    if(t1->isConstant() && !t2->isConstant()) {
        return swap12();
    }

    return nullptr;
}

Node *MulfNode::copy(Node *lhs, Node *rhs) {
    return alloc.new_object<MulfNode>(lhs, rhs);
}