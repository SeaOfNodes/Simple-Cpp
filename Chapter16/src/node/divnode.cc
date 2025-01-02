#include "../../Include/node/divnode.h"
#include "../../Include/type/integer_type.h"
#include "../../Include/node/divf_node.h"

DivNode::DivNode(Node *lhs, Node *rhs) : Node({nullptr, lhs, rhs}) {}

std::string DivNode::label() { return "Div"; }

std::string DivNode::glabel() { return "//"; }

std::ostringstream &DivNode::print_1(std::ostringstream &builder,
                                     Tomi::Vector<bool> &visited) {
    builder << "(";
    in(1)->print_0(builder, visited);
    builder << "/";
    in(2)->print_0(builder, visited);
    return builder;
}

Type *DivNode::compute() {
    Type*t1 = in(1)->type_;
    Type*t2 = in(2)->type_;
    if(t1->isHigh() || t2->isHigh()) return TypeInteger::TOP();

    auto i1 = dynamic_cast<TypeInteger *>(t1);
    auto i2 = dynamic_cast<TypeInteger *>(t2);
    if (i1 && i2) {
        if (i1->isConstant() && i2->isConstant()) {
            return i1->value() == 0
                   ? TypeInteger::ZERO()
                   : TypeInteger::constant(i1->value() / i2->value());
        }
    }
    return TypeInteger::BOT();
}

Node *DivNode::idealize() {
    if(in(2)->type_ == TypeInteger::TRUE()) return in(1);
    return nullptr;
}

Node *DivNode::copy(Node *lhs, Node *rhs) { return alloc.new_object<DivNode>(lhs, rhs); }
Node* DivNode::copyF() { return alloc.new_object<DivFNode>(nullptr, nullptr); }
