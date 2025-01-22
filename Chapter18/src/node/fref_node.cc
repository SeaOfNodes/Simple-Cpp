#include "../../Include/node/fref_node.h"
#include "../../Include/parser.h"

Type* FRefNode::FREF_TYPE = Type::BOTTOM();

FRefNode::FRefNode(ScopeMinNode::Var *n) : n_(n), ConstantNode(FREF_TYPE, Parser::START) {
    type_ = FREF_TYPE;
}

std::string FRefNode::label() {
    std::ostringstream os;
    return "FRef" + con_->print_1(os).str();
}

std::string FRefNode::uniqueName() {
    return "FRef_" + nid;
}

std::ostringstream &FRefNode::print_1(std::ostringstream &os, Tomi::Vector<bool> &visited) {
    os << con_->print_1(os).str();
    return os;
}

Node *FRefNode::idealize() {
    // When FRef finds its definition, idealize to it
    return nIns() == 1 ? nullptr : in(1);
}

bool FRefNode::eq(Node *n) {
    return this == n;
}

int FRefNode::hash() {
    return n_->idx_;
}