#include "../../Include/node/divf_node.h"
#include "../../Include/type/type_float.h"

DivFNode::DivFNode(Node *lhs, Node *rhs) : Node({nullptr, lhs, rhs}) {}

std::string DivFNode::label() { return "DivF"; }

std::string DivFNode::glabel() { return "/"; }

std::ostringstream &DivFNode::print_1(std::ostringstream &builder,
                                      Tomi::Vector<bool> &visited) {
    builder << "(";
    in(1)->print_0(builder, visited);
    builder << "/";
    in(2)->print_0(builder, visited);
    builder << ")";
    return builder;
}

Type *DivFNode::compute() {
    auto i0 = dynamic_cast<TypeFloat *>(in(1)->type_);
    auto i1 = dynamic_cast<TypeFloat *>(in(2)->type_);

    if (i0 && i1) {
        if (i0->isConstant() && i1->isConstant()) {
            return TypeFloat::constant(i0->value() / i1->value());
        }
    }
    return in(1)->type_->meet(in(2)->type_);
}

Node *DivFNode::idealize() {
    Node *lhs = in(1);
    Node *rhs = in(2);

    Type *t1 = lhs->type_;
    Type *t2 = rhs->type_;

    auto *i = dynamic_cast<TypeFloat *>(t2);

    // Div of 1
    if (t2->isConstant() && i && i->value() == 1) return lhs;

    return nullptr;
}

Node *DivFNode::copy(Node *lhs, Node *rhs) {
    return alloc.new_object<DivFNode>(lhs, rhs);
}