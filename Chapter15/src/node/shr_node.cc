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
    if (in(1)->type_->isHigh() || in(2)->type_->isHigh()) return TypeInteger::TOP();
    auto i1 = dynamic_cast<TypeInteger *>(in(1)->type_);
    auto i2 = dynamic_cast<TypeInteger *>(in(2)->type_);

    if (i1 && i2) {
        /// >>> is just unsigned right shift
        if (i1->isConstant() && i2->isConstant()) {
            return TypeInteger::constant(i1->value() >> i2->value());
        }
        // cant be negative or greater than 64
        if (i2->min_ < 0 || i2->max_ >= 64) {
            return TypeInteger::BOT();
        }
        // Zero shifting a negative makes a larger positive
        // so get the endpoints correct.
        // Todo: how do we know its right shift?
        long s1 = i1->min_ >> i2->min_;
        long s2 = i1->max_ >> i2->max_;
        bool wrap = i1->min_ < 0 && i1->max_ >= 0;
        return TypeInteger::make(wrap ? 0 : std::min(s1, s2), std::max(s1, s2));
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