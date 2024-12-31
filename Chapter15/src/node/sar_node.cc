#include "../../Include/node/sar_node.h"
#include "../../Include/type/integer_type.h"

SarNode::SarNode(Node *lhs, Node *rhs) :Node{nullptr, lhs, rhs}{}
std::string SarNode::label() { return "Sar"; }
std::string SarNode::glabel() { return ">>"; }

std::ostringstream &SarNode::print_1(std::ostringstream &builder, Tomi::Vector<bool> &visited) {
    builder << "(";
    in(1)->print_0(builder, visited);

    builder << ">>";
    in(2)->print_0(builder, visited);

    builder << ")";

    return builder;
}

Type*SarNode::compute() {
if(in(1)->type_->isHigh() || in(2)->type_->isHigh()) return TypeInteger::TOP();

    auto i1 = dynamic_cast<TypeInteger *>(in(1)->type_);
    auto i2 = dynamic_cast<TypeInteger *>(in(2)->type_);
    if(i1 && i2) {
        if(i1->isConstant() && i2->isConstant()) {
            return TypeInteger::constant(i1->value() >> i2->value());
        }
        if(i2->isConstant()) {
            int log = static_cast<long>(i2->value());
            return TypeInteger::make(static_cast<long>(-1LL << (63 - log)), static_cast<long>((1LL << (63 - log)) - 1));
        }

    }
    return TypeInteger::BOT();

}

Node* SarNode::idealize() {
    Node*lhs = in(1);
    Node*rhs = in(2);
    Type*t2 = rhs->type_;

    // Sar of 0
    auto*i = dynamic_cast<TypeInteger*>(t2);
    if(t2->isConstant() && i && (i->value() & 63) == 0) return lhs;


    // TODO: x >> 3 >> (y ? 1 : 2) ==> x >> (y ? 4 : 5)
    return nullptr;
}

Node *SarNode::copy(Node *lhs, Node *rhs) {
    return alloc.new_object<SarNode>(lhs, rhs);
}

std::string SarNode::err() {
    if(!dynamic_cast<TypeInteger*>(in(1)->type_))  return "Cannot '>>' " + in(1)->type_->str();
    if(!dynamic_cast<TypeInteger*>(in(2)->type_)) return "Cannot '>>' " + in(2)->type_->str();
    return "";
}
