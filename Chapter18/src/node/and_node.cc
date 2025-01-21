#include "../../Include/node/and_node.h"

#include "../../Include/node/add_node.h"

AndNode::AndNode(Lexer *loc, Node *lhs, Node *rhs) : LogicalNode(loc, lhs, rhs) {}

std::string AndNode::label() { return "And"; }

std::string AndNode::glabel() { return "&"; }

std::string AndNode::op() {
    return "&";
}

std::ostringstream &AndNode::print_1(std::ostringstream &builder, Tomi::Vector<bool> &visited) {
    builder << "(";
    in(1)->print_0(builder, visited);
    builder << "&";
    in(2)->print_0(builder, visited);
    builder << ")";
    return builder;
}

Type *AndNode::compute() {
    Type *t1 = in(1)->type_;
    Type *t2 = in(2)->type_;

    auto i0 = dynamic_cast<TypeInteger *>(t1);
    auto i1 = dynamic_cast<TypeInteger *>(t2);

    if (i0 && i1) {
        if (i0->isConstant() && i1->isConstant()) {
            return TypeInteger::constant(i0->value() & i1->value());
        }
        // Sharpen allowed bits if either value is narrowed
        long mask = i0->mask() & i1->mask();
        return mask < 0 ? TypeInteger::BOT() : TypeInteger::make(static_cast<long>(0), mask);
    }

    return TypeInteger::BOT();
}

Node *AndNode::idealize() {
    Node *lhs = in(1);
    Node *rhs = in(2);

    Type *t1 = lhs->type_;
    Type *t2 = rhs->type_;

    // And of -1.  We do not check for (-1&x) because this will already
    // canonicalize to (x&-1)
    auto *i = dynamic_cast<TypeInteger *>(t2);
    if (t2->isConstant() && i && i->value() == -1) return lhs;

    // Move constants to RHS: con*arg becomes arg*con
    if (t1->isConstant() && !t2->isConstant()) return swap12();

    // Do we have ((x & (phi cons)) & con) ?
    // Do we have ((x & (phi cons)) & (phi cons)) ?
    // Push constant up through the phi: x & (phi con0&con0 con1&con1...)
    Node *phicon = AddNode::phiCon(this, true);
    if (phicon != nullptr) return phicon;

    return nullptr;
}

Node *AndNode::copy(Node *lhs, Node *rhs) {
    return alloc.new_object<AndNode>(loc_, lhs, rhs);
}
//
//std::string AndNode::err() {
//    if(!dynamic_cast<TypeInteger*>(in(1)->type_)) return "Cannot '&' " + in(1)->type_->str();
//    if(!dynamic_cast<TypeInteger*>(in(2)->type_)) return "Cannot '&' " + in(2)->type_->str();
//    return "";
//}