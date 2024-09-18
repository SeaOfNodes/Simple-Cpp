#include "../../Include/node/proj_node.h"

ProjNode::ProjNode(MultiNode *ctrl, int idx, std::string label)
    : Node({ctrl}), idx_(idx), label_(label) {
}

std::string ProjNode::label() { return label_; }

std::ostringstream &ProjNode::print_1(std::ostringstream &builder) {
    builder << label_;
    return builder;
}

bool ProjNode::isCFG() {
    return idx_ == 0;
}

MultiNode *ProjNode::ctrl() {
    return (MultiNode *) in(0);
}

Type *ProjNode::compute() {
    Type *t = ctrl()->type_;
    if (auto tt = dynamic_cast<TypeTuple *>(t)) {
        return tt->types_[idx_];
    }
    return &Type::BOTTOM;
}

Node *Node::idealize() {
    return nullptr;
}
