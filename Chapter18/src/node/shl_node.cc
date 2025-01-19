#include "../../Include/node/shl_node.h"
#include "../../Include/type/integer_type.h"
#include "../../Include/node/add_node.h"
#include "../../Include/parser.h"
#include <limits>


ShlNode::ShlNode(Lexer loc, Node *lhs, Node *rhs) : LogicalNode(loc, lhs, rhs){}
std::string ShlNode::label() { return "Shl"; }
std::string ShlNode::glabel() { return "&lt;&lt;"; }

std::string ShlNode::op() {
    return "<<";
}

std::ostringstream &ShlNode::print_1(std::ostringstream &builder, Tomi::Vector<bool> &visited) {
    builder << "(";
    in(1)->print_0(builder, visited);

    builder << "<<";
    in(2)->print_0(builder, visited);

    builder << ")";

    return builder;
}

Type *ShlNode::compute() {
    Type*t1 = in(1)->type_;
    Type*t2 = in(2)->type_;

    if(t1->isHigh() || t2->isHigh()) return TypeInteger::TOP();

    auto i0 = dynamic_cast<TypeInteger *>(t1);
    auto i1 = dynamic_cast<TypeInteger *>(t2);
    if(i0 && i1) {
        if(i0 == TypeInteger::ZERO()) return TypeInteger::ZERO();
        if(i0->isConstant() && i1->isConstant()) {
            return TypeInteger::constant(i0->value() << i1->value());
        }
    }

    return TypeInteger::BOT();
}

Node* ShlNode::idealize() {
    Node*lhs = in(1);
    Node*rhs = in(2);
    Type*t2 = rhs->type_;

    // Shl of 0
    auto*shl = dynamic_cast<TypeInteger*>(t2);
    if(shl && shl->isConstant()) {
        // Shl of 0.
        // 1 << 0 => 1
        if((shl->value()&63) == 0) return lhs;

        // (x + c) << i  =>  (x << i) + (c << i)
        auto* add = dynamic_cast<AddNode*>(lhs);
        if(add) {
            auto*c = dynamic_cast<TypeInteger*>(add->addDep(this)->in(2)->type_);
            if(c && c->isConstant()) {
                long sum = c->value() << shl->value();
                if (std::numeric_limits<long>::min() <= sum && sum <= std::numeric_limits<long>::max()) {
                    return alloc.new_object<AddNode>(alloc.new_object<ShlNode>(loc_, add->in(1), rhs)->peephole(),
                                                     Parser::con(sum));
                }
            }
        }
}
    // TODO: x << 3 << (y ? 1 : 2) ==> x << (y ? 4 : 5)
    return nullptr;
}
Node *ShlNode::copy(Node *lhs, Node *rhs) { return alloc.new_object<ShlNode>(loc_, lhs, rhs); }
