#include "../../Include/node/bool_node.h"
#include "../../Include/parser.h"
#include "../../Include/type/type_float.h"
#include "../../Include/type/integer_type.h"

BoolNode::BoolNode(Node *lhs, Node *rhs) : Node({nullptr, lhs, rhs}) {}

std::string BoolNode::label() { return "Bool Node"; }

std::string BoolNode::glabel() { return op(); }

std::string BoolNode::op() { return "BoolNode"; }

TypeInteger *BoolNode::doOp(TypeInteger *lhs, TypeInteger *rhs) const { throw std::runtime_error("TODO!"); }

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
    Type *t1 = in(1)->type_;
    Type *t2 = in(2)->type_;
    // Exactly equals
    if (in(1) == in(2)) {
        // LT fails, both EQ and LE succeed.
        if (dynamic_cast<LT *>(this)) {
            return TypeInteger::FALSE();
        }
        return TypeInteger::TRUE();
    }
    if (t1->isHigh() || t2->isHigh()) return TypeInteger::BOOL()->dual();
    // Compare of same
    auto i0 = dynamic_cast<TypeInteger *>(t1);
    auto i1 = dynamic_cast<TypeInteger *>(t2);
    if (i0 && i1) {
        return doOp(i0, i1);
    }
    auto p0 = dynamic_cast<TypeFloat *>(t1);
    auto p1 = dynamic_cast<TypeFloat *>(t2);
    if (p0 && p1 && p0->isConstant() && p1->isConstant()) {
        return doOp(p0->value(), p1->value()) ? TypeInteger::TRUE() : TypeInteger::FALSE();
    }
    return TypeInteger::BOOL();
}

Node *BoolNode::idealize() {
    // compare of Same(pointer comparison)
    if (in(1) == in(2)) {
        if (dynamic_cast<LT *>(this)) {
            return Parser::ZERO;
        }
        return alloc.new_object<ConstantNode>(TypeInteger::TRUE(), Parser::START);
    }

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
        if (in(2)->type_ == TypeInteger::ZERO() || in(2)->type_ == TypeMemPtr::NIL()) {
            return alloc.new_object<NotNode>(in(1));
        }


        // Do we have ((x * (phi cons)) * con) ?
        // Do we have ((x * (phi cons)) * (phi cons)) ?
        // Push constant up through the phi: x * (phi con0*con0 con1*con1...)
        Node *phicon = AddNode::phiCon(this, dynamic_cast<EQ *>(this));
        if (phicon != nullptr) return phicon;
    }
    return nullptr;
}

// EQ
EQ::EQ(Node *lhs, Node *rhs) : BoolNode(lhs, rhs) {}

Node *EQ::copy(Node *lhs, Node *rhs) { return alloc.new_object<EQ>(lhs, rhs); }

std::string EQ::label() { return "EQ"; }

std::string EQ::op() { return "=="; }

TypeInteger *EQ::doOp(TypeInteger *i1, TypeInteger *i2) const {
    if (i1 == i2 && i1->isConstant()) return TypeInteger::TRUE();
    if (i1->max_ < i2->min_ || i1->min_ > i2->max_) return TypeInteger::FALSE();
    return TypeInteger::BOOL();
}

// LT
LT::LT(Node *lhs, Node *rhs) : BoolNode(lhs, rhs) {}

Node *LT::copy(Node *lhs, Node *rhs) { return alloc.new_object<LT>(lhs, rhs); }

std::string LT::glabel() {
    return "&lt;=";
}

std::string LT::label() { return "LT"; }

std::string LT::op() { return "<"; }

TypeInteger *LT::doOp(TypeInteger *i1, TypeInteger *i2) const {
    if (i1->max_ < i2->min_) return TypeInteger::TRUE();
    if (i1->min_ >= i2->max_) return TypeInteger::FALSE();
    return TypeInteger::BOOL();
}

// LE
LE::LE(Node *lhs, Node *rhs) : BoolNode(lhs, rhs) {}

Node *LE::copy(Node *lhs, Node *rhs) { return alloc.new_object<LE>(lhs, rhs); }

std::string LE::label() { return "LE"; }

std::string LE::op() { return "<="; }

std::string LE::glabel() {
    return "&lt;=";
}

TypeInteger *LE::doOp(TypeInteger *i1, TypeInteger *i2) const {
    if (i1->max_ <= i2->min_) return TypeInteger::TRUE();
    if (i1->min_ > i2->max_) return TypeInteger::FALSE();
    return TypeInteger::BOOL();
}

EQF::EQF(Node *lhs, Node *rhs) : EQ(lhs, rhs) {}

bool EQF::doOp(double lhs, double rhs) const {
    return lhs == rhs;
}

std::string EQF::label() {
    return "EQF";
}

Node *EQF::copy(Node *lhs, Node *rhs) {
    return alloc.new_object<EQF>(lhs, rhs);
}

LTF::LTF(Node *lhs, Node *rhs) : LT(lhs, rhs) {}

bool LTF::doOp(double lhs, double rhs) const {
    return lhs < rhs;
}

std::string LTF::label() {
    return "LTF";
}

Node *LTF::copy(Node *lhs, Node *rhs) {
    return alloc.new_object<LTF>(lhs, rhs);
}

LEF::LEF(Node *lhs, Node *rhs) : LE(lhs, rhs) {}

bool LEF::doOp(double lhs, double rhs) const {
    return lhs <= rhs;
}

Node *LEF::copy(Node *lhs, Node *rhs) {
    return alloc.new_object<LEF>(lhs, rhs);
}

std::string LEF::label() {
    return "LEF";
}


