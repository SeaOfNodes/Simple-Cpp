#include "../../Include/node/proj_node.h"
#include "../../Include/type/type_mem.h"

ProjNode::ProjNode(Node *ctrl, int idx, std::string label)
        : Node({ctrl}), idx_(idx), label_(std::move(label)) {}

std::string ProjNode::label() { return label_; }

bool ProjNode::isMultiTail() { return in(0)->isMultiHead(); }

std::ostringstream &ProjNode::print_1(std::ostringstream &builder,
                                      Tomi::Vector<bool> &visited) {
    builder << label_;
    return builder;
}

bool ProjNode::isPinned() {
    return true;
}
bool ProjNode::isCFG() { return idx_ == 0 || dynamic_cast<IfNode *>(ctrl()); }

MultiNode *ProjNode::ctrl() { return (MultiNode *) in(0); }

bool ProjNode::isMem() {
    auto *t = dynamic_cast<TypeMem *>(type_);
    return t != nullptr;
}

Type *ProjNode::compute() {
    Type *t = in(0)->type_;
    if (auto tt = dynamic_cast<TypeTuple *>(t); tt) {
        return tt->types_[idx_];
    }
    return Type::BOTTOM();
}

Node *ProjNode::idealize() {
    // Todo: why get rid of this
//    if (auto *iff = dynamic_cast<IfNode *>(ctrl())) {
//        if (auto *tt = dynamic_cast<TypeTuple *>(iff->type_);
//                (tt) && (tt->types_[1 - idx_] == Type::XCONTROL())) {
//            return iff->in(0); // We become our input control
//        }
//        // Flip a negating if-test,to remove the not
//        if (auto *NOT = dynamic_cast<NotNode *>(iff->pred()->addDep(this))) {
//            return new ProjNode((MultiNode *) (new IfNode(iff->ctrl(), NOT->in(1)))->peephole(), 1 - idx_,
//                                idx_ == 0 ? "False" : "True");
//        }
//    }

    return nullptr;
}

bool ProjNode::eq(Node *n) {
    if (!dynamic_cast<ProjNode *>(n)) {
        std::cerr << "messed up";
    }
    return idx_ == dynamic_cast<ProjNode *>(n)->idx_;
}

int ProjNode::hash() { return idx_; }
