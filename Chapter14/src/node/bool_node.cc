#include "../../Include/node/bool_node.h"
#include "../../Include/parser.h"
#include "../../Include/type/type_float.h"

BoolNode::BoolNode(Node *lhs, Node *rhs) : Node({nullptr, lhs, rhs}) {}

std::string BoolNode::label() { return "Bool Node"; }

std::string BoolNode::glabel() { return op(); }

std::string BoolNode::op() { return "BoolNode"; }

bool BoolNode::doOp(long lhs, long rhs) const { throw std::runtime_error("TODO!"); }

bool BoolNode::doOp(double lhs, double rhs) const { throw std::runtime_error("TODO!"); }

Node *BoolNode::copyF(Node *lhs, Node *rhs) {
    throw std::runtime_error("TODO!");
}

std::ostringstream &BoolNode::print_1(std::ostringstream &builder,
                                      Tomi::Vector<bool> &visited) {
    builder << "(";
    in(1)->print_0(builder, visited);
    builder << op();
    in(2)->print_0(builder, visited);
    builder << ")";
    return builder;
}

Type *BoolNode::compute() {
    auto i0 = dynamic_cast<TypeInteger *>(in(1)->type_);
    auto i1 = dynamic_cast<TypeInteger *>(in(2)->type_);
    if (i0 && i1) {
        if (i0->isConstant() && i1->isConstant())
            return TypeInteger::constant(doOp(i0->value(), i1->value()) ? 1 : 0);
        return i0->meet(i1);
    }
    auto p0 = dynamic_cast<TypeFloat *>(in(1)->type_);
    auto p1 = dynamic_cast<TypeFloat *>(in(2)->type_);
    if (p0 && p1) {
        if (p0->isConstant() && p1->isConstant())
            return TypeInteger::constant(doOp(p0->value(), p1->value()) ? 1 : 0);
        return p0->meet(p1);
    }
    return in(1)->type_->meet(in(2)->type_);
}

Node *BoolNode::idealize() {
    // compare of Same(pointer comparison)
    if (in(1) == in(2))

        return alloc.new_object<ConstantNode>(TypeInteger::constant(doOp(static_cast<long>(3), static_cast<long>(3)) ? 1 : 0),
                                              Parser::START);
    // Equals pushes constant to the right; 5 == x becomes x == 5
    if (dynamic_cast<EQ *>(this)) {
        if (!dynamic_cast<ConstantNode *>(in(2))) {
            // con == noncon becomes noncon ==con
            if (dynamic_cast<ConstantNode *>(in(1)) || in(1)->nid > in(2)->nid) {
                if (dynamic_cast<TypeFloat *>(in(1)->type_)) {
                    return alloc.new_object<EQF>(in(2), in(1));
                }
                return alloc.new_object<EQ>(in(2), in(1));
            }
            // Equals sorts by NID oterwise: non.high == non.low becomes non.low == non.high

        }
        // Equals X==0 becomes a !X
        if (in(2)->type_ == TypeInteger::ZERO() || in(2)->type_ == TypeMemPtr::NULLPTR()) {
            return alloc.new_object<NotNode>(in(1));
        }
    }
    // Do we have ((x * (phi cons)) * con) ?
    // Do we have ((x * (phi cons)) * (phi cons)) ?
    // Push constant up through the phi: x * (phi con0*con0 con1*con1...)

    Node *phicon = AddNode::phiCon(this, false);
    if (phicon != nullptr)
        return phicon;
    return nullptr;
}

// EQ
EQ::EQ(Node *lhs, Node *rhs) : BoolNode(lhs, rhs) {}

Node *EQ::copy(Node *lhs, Node *rhs) { return alloc.new_object<EQ>(lhs, rhs); }

std::string EQ::label() { return "EQ"; }

std::string EQ::op() { return "=="; }

bool EQ::doOp(long lhs, long rhs) const { return lhs == rhs; }

// LT
LT::LT(Node *lhs, Node *rhs) : BoolNode(lhs, rhs) {}

Node *LT::copy(Node *lhs, Node *rhs) { return alloc.new_object<LT>(lhs, rhs); }

std::string LT::label() { return "LT"; }

std::string LT::op() { return "<"; }

bool LT::doOp(long lhs, long rhs) const { return lhs < rhs; }

// LE
LE::LE(Node *lhs, Node *rhs) : BoolNode(lhs, rhs) {}

Node *LE::copy(Node *lhs, Node *rhs) { return alloc.new_object<LE>(lhs, rhs); }

std::string LE::label() { return "LE"; }

std::string LE::op() { return "<="; }

bool LE::doOp(long lhs, long rhs) const { return lhs <= rhs; }

EQF::EQF(Node *lhs, Node *rhs) : EQ(lhs, rhs) {}

bool EQF::doOp(double lhs, double rhs) const {
    return lhs == rhs;
}

std::string EQF::label() {
    return "EQF";
}

Node *EQF::copyF() {
    return nullptr;
}

LTF::LTF(Node *lhs, Node *rhs) : LT(lhs, rhs){}
bool LTF::doOp(double lhs, double rhs) const {
    return lhs < rhs;
}

std::string LTF::label() {
    return "LTF";
}

Node *LTF::copyF() {
    return nullptr;
}

LEF::LEF(Node *lhs, Node *rhs) : LE(lhs, rhs){}
bool LEF::doOp(double lhs, double rhs) const {
    return lhs <= rhs;
}

Node* LEF::copyF() {
    return nullptr;
}

std::string LEF::label() {
    return "LEF";
}


