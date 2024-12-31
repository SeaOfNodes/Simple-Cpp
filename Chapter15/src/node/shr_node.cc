#include "../Include/node/shr_node.h"
#include "../Include/type/integer_type.h"

ShrNode::ShrNode(Node *lhs, Node *rhs) : Node({nullptr, lhs, rhs}) {}

std::string ShrNode::label() {
    return "Shr";
}

std::string ShrNode::glabel() {
    return ">>>";
}

std::ostringstream &ShrNode::print_1(std::ostringstream &builder, Tomi::Vector<bool> &visited) {
    builder << "(";
    in(1)->print_0(builder, visited);

    builder << ">>>";
    in(2)->print_0(builder, visited);

    builder << ")";

    return builder;
}

Type *ShrNode::compute() {
    auto i1 = dynamic_cast<TypeInteger *>(in(1)->type_);
    auto i2 = dynamic_cast<TypeInteger *>(in(2)->type_);

    if (i1 && i2) {
        if (i1->isConstant() && i2->isConstant()) {
            return TypeInteger::constant(i1->value() >> i2->value());
        }
    }
    return TypeInteger::BOT();
}

Node *ShrNode::idealize() {
    Node *lhs = in(1);
    Node *rhs = in(2);

    Type *t2 = rhs->type_;

    // Shr of 0
    https://docs.oracle.com/javase/specs/jls/se15/html/jls-15.html#jls-15.19
    auto *i = dynamic_cast<TypeInteger *>(t2);
    if (t2->isConstant() && i && (i->value() & 63) == 0) return lhs;
    return nullptr;

}

Node *ShrNode::copy(Node *lhs, Node *rhs) {
    return alloc.new_object<ShrNode>(lhs, rhs);
}