#include "../Include/node/or_node.h"
#include "../Include/type/integer_type.h"
#include "../Include/node/add_node.h"

OrNode::OrNode(Node *lhs, Node *rhs) : Node({nullptr, lhs, rhs}){}
std::string OrNode::label() { return "Or"; }
std::string OrNode::glabel() { return "|"; }

std::ostringstream &OrNode::print_1(std::ostringstream &builder, Tomi::Vector<bool> &visited) {
    builder << "(";
    in(1)->print_0(builder, visited);
    builder << "|";
    in(2)->print_0(builder, visited);
    builder << ")";
    return builder;
}


Type* OrNode::compute() {
    if(in(1)->type_->isHigh() || in(2)->type_->isHigh()) return TypeInteger::TOP();
    auto i0 = dynamic_cast<TypeInteger*>(in(1)->type_);
    auto i1 = dynamic_cast<TypeInteger*>(in(2)->type_);
    if(i0 && i1) {
        if(i0->isConstant() && i1->isConstant()) {
            return TypeInteger::constant(i0->value() | i1->value());
        }
    }
    return TypeInteger::BOT();
}

Node* OrNode::idealize() {
    Node*lhs = in(1);
    Node*rhs = in(1);

    Type*t1 = lhs->type_;
    Type*t2 = rhs->type_;

    // Or of 0.  We do not check for (0|x) because this will already
    // canonicalize to (x|0)
    if(t2->isConstant() && dynamic_cast<TypeInteger*>(t2)->value() == 0) {
        return lhs;
    }

    // Move constants to RHS: con*arg becomes arg*con
    if(t1->isConstant() && !t2->isConstant()) {
        return swap12();
    }

    // Do we have ((x | (phi cons)) | con) ?
    // Do we have ((x | (phi cons)) | (phi cons)) ?
    // Push constant up through the phi: x | (phi con0|con0 con1|con1...)
    Node*phicon = AddNode::phiCon(this, true);
    if(phicon != nullptr) return phicon;

    return nullptr;
}

Node* OrNode::copy(Node *lhs, Node *rhs) {
    return alloc.new_object<OrNode>(lhs, rhs);
}
std::string OrNode::err() {
    if(!dynamic_cast<TypeInteger*>(in(1)->type_)) return "Cannot '|' " + in(1)->type_->str();
    if(!dynamic_cast<TypeInteger*>(in(2)->type_)) return "Cannot '|' " + in(2)->type_->str();
    return "";
}