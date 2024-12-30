#include "../../Include/node/shl_node.h"
#include "../../Include/type/integer_type.h"

ShlNode::ShlNode(Node *lhs, Node *rhs) : Node{nullptr, lhs, rhs}{}
std::string ShlNode::label() { return "Shl"; }
std::string ShlNode::glabel() { return "<<"; }

std::ostringstream &ShlNode::print_1(std::ostringstream &builder, Tomi::Vector<bool> &visited) {
    builder << "(";
    in(1)->print_0(builder, visited);

    builder << "<<";
    in(2)->print_0(builder, visited);

    builder << ")";

    return builder;
}

Type *ShlNode::compute() {
    if(in(1)->type_->isHigh() || in(2)->type_->isHigh()) return TypeInteger::TOP();

    auto i0 = dynamic_cast<TypeInteger *>(in(1)->type_);
    auto i1 = dynamic_cast<TypeInteger *>(in(2)->type_);
    if(i0->isConstant() && i1->isConstant()) {
        return TypeInteger::constant(i0->value() << i1->value());
    }

    return TypeInteger::BOT();
}

Node* ShlNode::idealize() {
    Node*lhs = in(1);
    Node*rhs = in(2);
    Type*t2 = rhs->type_;

    // Shl of 0
    auto*i = dynamic_cast<TypeInteger*>(t2);
    if(t2->isConstant() && i && (i->value() & 63) == 0) return lhs;

    // TODO: x << 3 << (y ? 1 : 2) ==> x << (y ? 4 : 5)
    return nullptr;
}
Node *ShlNode::copy(Node *lhs, Node *rhs) { return alloc.new_object<ShlNode>(lhs, rhs); }

std::string ShlNode::err() {
    if(!dynamic_cast<TypeInteger*>(in(1)->type_))  return "Cannot '<<' " + in(1)->type_->str();
    if(!dynamic_cast<TypeInteger*>(in(2)->type_)) return "Cannot '<<' " + in(2)->type_->str();
    return "";
}