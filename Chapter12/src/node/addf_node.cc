#include "../../Include/node/addf_node.h"
#include "../../Include/type/type_float.h"

AddFNode::AddFNode(Node *lhs, Node *rhs) : Node({nullptr, lhs, rhs}) {}
std::string AddFNode::label() { return "AddF"; }
std::string AddFNode::glabel() { return "+"; }

std::ostringstream &AddFNode::print_1(std::ostringstream &builder,
                                     Tomi::Vector<bool> &visited) {
  builder << "(";
  in(1)->print_0(builder, visited);
  builder << "+";
  in(2)->print_0(builder, visited);
  builder << ")";
  return builder;
}

Type *AddFNode::compute() {
    auto i0 = dynamic_cast<TypeFloat *>(in(1)->type_);
    auto i1 = dynamic_cast<TypeFloat *>(in(2)->type_);

    if (i0 && i1) {
        if (i0->isConstant() && i1->isConstant()) {
            return TypeFloat::constant(i0->value() + i1->value());
        }
    }
    return in(1)->type_->meet(in(2)->type_);
}

Node *AddFNode::idealize() {
    Node*lhs = in(1);
    Type*t2 = in(2)->type_;

    auto*i = dynamic_cast<TypeFloat*>(t2);
    // Add of 0.
    if(t2->isConstant() && i && i->value() == 0) return lhs;

    return nullptr;
}
Node* AddFNode::copy(Node *lhs, Node *rhs) {
    return alloc.new_object<AddFNode>(lhs, rhs);
}
