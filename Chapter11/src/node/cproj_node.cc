#include "../../Include/node/cproj_node.h"
#include "../../Include/node/node.h"
#include "../../Include/type/tuple_type.h"
#include "../../Include/node/if_node.h"
#include "../../Include/node/not_node.h"

CProjNode::CProjNode(Node *ctrl, int idx, std::string label) : CFGNode({ctrl}), idx_(idx), label_(label) {
}

std::string CProjNode::label() {
    return label_;
}

std::ostringstream &CProjNode::print_1(std::ostringstream &builder, Tomi::Vector<bool> &visited) {
    builder << label_;
    return builder;
}

bool CProjNode::isMultiTail() {
    return in(0)->isMultiHead();
}

bool CProjNode::blockHead() {
    return true;
}

CFGNode *CProjNode::ctrl() {
    return cfg(0);
}

Type *CProjNode::compute() {
    Type *t = ctrl()->type_;
    if (auto tt = dynamic_cast<TypeTuple *>(t); tt) {
        return tt->types_[idx_];
    }
    return Type::BOTTOM();
}

Node *CProjNode::idealize() {
    if (auto *iff = dynamic_cast<IfNode *>(ctrl())) {
        if (auto *tt = dynamic_cast<TypeTuple *>(iff->type_);
                (tt) && (tt->types_[1 - idx_] == Type::XCONTROL())) {
            return iff->in(0); // We become our input control
        }
        // Flip a negating if-test,to remove the not
        if (auto *NOT = dynamic_cast<NotNode *>(iff->pred()->addDep(this))) {
            return new ProjNode((MultiNode *) (new IfNode(iff->ctrl(), NOT->in(1)))->peephole(), 1 - idx_,
                                idx_ == 0 ? "False" : "True");
        }
    }

    return nullptr;
}

Node *CProjNode::getBlockStart() {
    if (dynamic_cast<IfNode *>(ctrl())) {
        return this;
    }
    return ctrl()->getBlockStart();
}

bool CProjNode::eq(Node *n) {
    if (!dynamic_cast<CProjNode *>(n)) {
        std::cerr << "messed up";
    }
    return idx_ == dynamic_cast<CProjNode *>(n)->idx_;
}

int CProjNode::hash() {
    return idx_;
}