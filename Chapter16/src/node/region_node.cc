#include "../../Include/node/region_node.h"
#include "../../Include/node/constant_node.h"

#include "../../Include/node/cproj_node.h"

#include "../../Include/parser.h"
#include <algorithm>

RegionNode::RegionNode(std::initializer_list<Node *> nodes) : CFGNode(nodes) {}
std::string RegionNode::label() { return "Region"; }

std::ostringstream &RegionNode::print_1(std::ostringstream &builder,
                                        Tomi::Vector<bool> &visited) {
  builder << label();
  builder << nid;
  return builder;
}

bool RegionNode::isCFG() { return true; }

bool RegionNode::blockHead() {
    return true;
}
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
      return Parser::XCTRL;
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
  auto *p1 = dynamic_cast<CProjNode*>(in(1));
  auto *p2 = dynamic_cast<CProjNode*>(in(2));
  if(!hashPhi() && p1 && p2 && p1->in(0)->addDep(this) == p2->in(0)->addDep(this) && dynamic_cast<IfNode*>(p1->in(0))) {
      return dynamic_cast<IfNode*>(p1->in(0))->ctrl();
  }
  // TOdo: guarded expression here
  return nullptr;
}

int RegionNode::loopDepth() {
    return loopDepth_ == 0 ? (loopDepth_ = cfg(1)->loopDepth()) : loopDepth_;
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
            d = std::max(d, ((CFGNode*)n)->idepth());
        }
    }
    idepth_ = d;
    return d;
}
CFGNode *RegionNode::idom(Node* dep) {
CFGNode* lca = nullptr;

for(int i = 1; i< nIns(); i++) {
    lca = cfg(i)->idom(lca, dep);
}
return lca;
}

Node* RegionNode::getBlockStart() {
    return this;
}
void RegionNode::walkUnreach_(Tomi::BitArray<10> &visit, Tomi::HashSet<CFGNode*>& unreach){
    for(int i =1; i <nIns(); i++) {
        cfg(i)->walkUnreach(visit, unreach);
    }
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