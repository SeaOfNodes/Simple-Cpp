#include "../../Include/node/region_node.h"
#include "../../Include/node/constant_node.h"
#include <algorithm>

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
  // Delete dead paths into a Region
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
    std::size_t nouts = 0;
    while (nouts != nOuts()) {
      nouts = nOuts();
      for (int i = 0; i < nouts; i++) {
        if (auto phi = dynamic_cast<PhiNode *>(out(i));
            phi && phi->nIns() == nIns()) {
          phi->delDef(path);
          IterPeeps::addAll(phi->outputs);
        }
      }
    }
    idom_ = nullptr;
    if (isDead())
      return alloc.new_object<ConstantNode>(Type::XCONTROL(), Parser::START);
    else
      return delDef(path);
  }
  // for (Node *phi : outputs) {
  //   if (dynamic_cast<PhiNode *>(phi)) {
  //     phi->delDef(path);
  //   }
  // }

  // If down to a single input, become that input - but also make all
  // Phis an identity on *their* single input.
  if (nIns() == 2 && !hashPhi()) {
    idom_ = nullptr;
    return in(1);
  }
  // IF a CFG diamond with no merging, delete: "if(pred) {} else {};
  auto *p1 = dynamic_cast<ProjNode*>(in(1));
  auto *p2 = dynamic_cast<ProjNode*>(in(2));
  if(!hashPhi() && p1 && p2 && p1->in(0) == p2->in(0) && dynamic_cast<IfNode*>(p1->in(0))) {
      return dynamic_cast<IfNode*>(p1->in(0))->ctrl();
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

int RegionNode::idepth() {
    if(idepth_ != 0) return idepth_;
    int d = 0;
    for(Node* n: inputs) {
        if(n != nullptr) {
            d = std::max(d, n->idepth());
        }
    }
    idepth_ = d;
    return d;
}
Node *RegionNode::idom() {
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
    auto comp = lhs->idepth() - rhs->idepth();
    if (comp >= 0)
      lhs = lhs->idom();
    if (comp <= 1)
      rhs = rhs->idom();
  }
  if (lhs == nullptr)
    return nullptr;
  idepth_ = lhs->idepth_ + 1;
  idom_ = lhs;
  return idom_;
}
bool RegionNode::inProgress() {
  return (nIns() > 1) && (in(nIns() - 1) == nullptr);
}
bool RegionNode::hashPhi() {
  for (Node *phi : outputs) {
    if (dynamic_cast<PhiNode *>(phi))
      return true;
  }
  return false;
}

bool RegionNode::eq(Node *n) { return !inProgress(); }