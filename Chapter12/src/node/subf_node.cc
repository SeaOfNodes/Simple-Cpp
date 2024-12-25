#include "../../Include/node/subf_node.h"

SubfNode::SubfNode(Node *lhs, Node *rhs) : Node({nullptr, lhs, rhs}) {
}

std::string SubfNode::label() {
    return "SubF";
}

std::string SubfNode::glabel() {
    return "-";
}

std::ostringstream &SubfNode::print_1(std::ostringstream &builder, Tomi::Vector<bool> &visited) {
    builder << "(";
    in(1)->print_0(builder, visited);
    builder << "-";
    in(2)->print_0(builder, visited);
    return builder;
}

Type *SubfNode::compute() {
    auto i0 = dynamic_cast<TypeInteger *>(in(1)->type_);
    auto i1 = dynamic_cast<TypeInteger *>(in(2)->type_);
    if (i0 && i1) {
        if (i0->isConstant() && i1->isConstant()) {
            return TypeFloat::constant(i0->value() - i1->value());
        }
    }
    return in(1)->type_->meet(in(2)->type_);
}

Node *SubfNode::idealize() {
    // -(-x) is x
    Node*lhs = in(1);
    Type*t2 = in(2)->type_;

    auto*i = dynamic_cast<TypeFloat*>(t2);
    // sub of 0
    if(t2->isConstant() && i && i->value() == 0) {
        return lhs;
    }
    return nullptr;
}

Node *SubfNode::copy(Node *lhs, Node *rhs) {
    return alloc.new_object<SubfNode>(lhs, rhs);
}