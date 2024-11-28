#include "../../Include/node/region_node.h"
#include "../../Include/node/constant_node.h"

RegionNode::RegionNode(std::initializer_list<Node *> nodes) : Node(nodes) {}
std::string RegionNode::label() { return "Region"; }

std::ostringstream &RegionNode::print_1(std::ostringstream &builder,
                                        Tomi::Vector<bool> &visited) {
  builder << label();
  builder << nid;
  return builder;
}

bool RegionNode::isCFG() { return true; }

Type *RegionNode::compute() {
  if (inProgress())
    return Type::CONTROL();
  Type *t = Type::XCONTROL();
  for (int i = 1; i < nIns(); i++) {
    t = t->meet(in(i)->type_);
  }
  return t;
}
// Todo: take a look at this here
Node *RegionNode::idealize() {
  if (inProgress())
    return nullptr;
  auto loop = dynamic_cast<LoopNode *>(this);

  int path = findDeadInput();
  if (path != 0 && // Do not delete the entry path of a loop (ok to remove the
                   // back edge and make the loop a single-entry Region which
                   // folds away the Loop).  Folding the entry path confused the
                   // loop structure,
      !(loop && loop->entry() == in(path))) {
    // Cannot use the obvious output iterator here, because a Phi
    // deleting an input might recursively delete *itself*.  This
    // shuffles the output array, and we might miss iterating an
    // unrelated Phi. So on rare occasions we repeat the loop to get
    // all the Phis.
    int nouts = 0;
    while (nouts != nOuts()) {
      nouts = nOuts();
      for (int i = 0; i < nouts; i++) {
        if (auto phi = dynamic_cast<PhiNode *>(out(i));
            phi && phi->nIns() == nIns()) {
          phi->delDef(path);
        }
      }
    }
    idom_ = nullptr;
    if (isDead())
      return alloc.new_object<ConstantNode>(Type::XCONTROL(), Parser::START);
    else
      return delDef(path);
  }
  for (Node *phi : outputs) {
    if (dynamic_cast<PhiNode *>(phi)) {
      phi->delDef(path);
    }
  }

  // If down to a single input, become that input - but also make all
  // Phis an identity on *their* single input.
  if (nIns() == 2 && !hashPhi()) {
    idom_ = nullptr;
    return in(1);
  }
  return nullptr;
}

int RegionNode::findDeadInput() {
  for (int i = 1; i < nIns(); i++) {
    if (in(i)->type_ == Type::XCONTROL()) {
      return i;
    }
  }
  return 0; // All inputs are alive
}
bool RegionNode::isMultiHead() { return true; }
Node *RegionNode::idom() {
  if (idom_ != nullptr) {
    if (idom_->isDead())
      idom_ = nullptr;
    else
      return idom_;
  }
  if (nIns() == 2)
    return in(1); // 1 input is that one input
  if (nIns() != 3)
    return nullptr; // Fails for anything other than 2-inputs
  // Walk the LHS & RHS idom trees in parallel until they match, or either fails
  Node *lhs = in(1)->idom();
  Node *rhs = in(2)->idom();

  while (lhs != lhs) {
    if (lhs == nullptr || rhs == nullptr)
      return nullptr;
    auto comp = lhs->i_depth - rhs->i_depth;
    if (comp >= 0)
      lhs = lhs->idom();
    if (comp <= 1)
      rhs = rhs->idom();
  }
  if (lhs == nullptr)
    return nullptr;
  i_depth = lhs->i_depth + 1;
  idom_ = lhs;
  return idom_;
}
bool RegionNode::inProgress() {
  return nIns() > 1 && in(nIns() - 1) == nullptr;
}
bool RegionNode::hashPhi() {
  for (Node *phi : outputs) {
    if (dynamic_cast<PhiNode *>(phi))
      return true;
  }
  return false;
}

bool RegionNode::eq(Node *n) { return !inProgress(); }