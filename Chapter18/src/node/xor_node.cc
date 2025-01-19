#include "node/xor_node.h"
#include "node/add_node.h"

#include "../Include/type/integer_type.h"
#include "../../Include/parser.h"

XorNode::XorNode(Lexer loc_, Node *lhs, Node *rhs) : Node({loc_, lhs, rhs}) {
}

std::string XorNode::label() {
    return "XOR";
}

std::string XorNode::glabel() {
    return "^";
}

std::string XorNode::op() {
    return "^";
}

std::ostringstream &XorNode::print_1(std::ostringstream &builder, Tomi::Vector<bool> &visit) {
    builder << "(";
    in(1)->print_0(builder, visit);
    builder << "^";
    in(2)->print_0(builder, visit);
    builder << ")";

    return builder;
}

Type *XorNode::compute() {
    // Todo: got rid of this, why?
    // if (in(1)->type_->isHigh() || in(2)->type_->isHigh()) return TypeInteger::TOP();

    Type*t1 = in(1)->type_;
    Type*t2 = in(2)->type_;
    if(t1->isHigh() || t2->isHigh()) {
        return TypeInteger::TOP();
    }
    auto i0 = dynamic_cast<TypeInteger *>(in(1)->type_);
    auto i1 = dynamic_cast<TypeInteger *>(in(2)->type_);


    if (i0 && i1) {
        if (i0->isConstant() && i1->isConstant()) {
            return TypeInteger::constant(i0->value() ^ i1->value());
        }
    }
    return TypeInteger::BOT();
}

Node* XorNode::idealize() {
    Node*lhs = in(1);
    Node*rhs = in(2);
    Type*t1 = lhs->type_;
    Type*t2 = rhs->type_;

    // Xor of 0.  We do not check for (0^x) because this will already
    // canonicalize to (x^0)
    auto*i = dynamic_cast<TypeInteger*>(t2);
    if(t2->isConstant() && i->value() == 0) {
        return lhs;
    }
    // Move constants to RHS: con*arg becomes arg*con
    if(t1->isConstant() && !t2->isConstant()) {
        return swap12();
    }
    // Do we have ((x ^ (phi cons)) ^ con) ?
    // Do we have ((x ^ (phi cons)) ^ (phi cons)) ?
    // Push constant up through the phi: x ^ (phi con0^con0 con1^con1...)
    Node* phicon = AddNode::phiCon(this,true);
    if(phicon != nullptr) return phicon;

    return nullptr;
}

Node* XorNode::copy(Node *lhs, Node *rhs) {
    return alloc.new_object<XorNode>(lhs, rhs);
}
