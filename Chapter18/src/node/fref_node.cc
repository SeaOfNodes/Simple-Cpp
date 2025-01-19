#include "../../Include/node/fref_node.h"

FrefNode::FrefNode(ScopeMinNode::Var* n) : n_(n), FrefNode(FREF_TYPE) {
    type_ = FREF_TYPE;
}

std::string FrefNode::label() {
    return "FRef" + n_;
}
std::string FrefNode::uniqueName() {
    return "FRef_" + nid;
}
std::ostringstream& print_1(std::ostringstream& os, Tomi::BitArray<10>& visited) {
    os << "FRef_" << n_;
    return os;
}

Node* FrefNode::idealize() {
    // When FRef finds its definition, idealize to it
    return nIns() == 1 ? nullptr: in(1);
}

bool FrefNode::eq(Node*n) {
    return this == n;
}
int FrefNode::hash() {
    return n_->idx_:
}