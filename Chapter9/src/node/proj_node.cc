#include "../../Include/node/proj_node.h"

ProjNode::ProjNode(MultiNode *ctrl, int idx, std::string label)
    : Node({ctrl}), idx_(idx), label_(std::move(label)) {}

std::string ProjNode::label() { return label_; }

bool ProjNode::isMultiTail() { return in(0)->isMultiHead(); }

std::ostringstream &ProjNode::print_1(std::ostringstream &builder,
                                      Tomi::Vector<bool> &visited) {
  builder << label_;
  return builder;
}

bool ProjNode::isCFG() { return idx_ == 0 || dynamic_cast<IfNode *>(ctrl()); }

MultiNode *ProjNode::ctrl() { return (MultiNode *)in(0); }

Type *ProjNode::compute() {
  Type *t = in(0)->type_;
  if (auto tt = dynamic_cast<TypeTuple *>(t); tt) {
    return tt->types_[idx_];
  }
  return Type::BOTTOM();
}

Node *ProjNode::idealize() {
  if (auto *tt = dynamic_cast<TypeTuple *>(ctrl()->type_);
          (tt) && (tt->types_[1 - idx_] == Type::XCONTROL())) {
    return ctrl()->in(0); // We become our input control
  }
  return nullptr;
}

bool ProjNode::eq(Node *n) {
  if (!dynamic_cast<ProjNode *>(n)) {
    std::cerr << "messed up";
  }
  return idx_ == dynamic_cast<ProjNode *>(n)->idx_;
}
int ProjNode::hash() { return idx_; }
