#include "../../Include/node/add_node.h"

AddNode::AddNode(Node *lhs, Node *rhs) : Node({nullptr, lhs, rhs}) {}

std::string AddNode::label() { return "Add"; }
std::string AddNode::glabel() { return "+"; }

std::ostringstream &AddNode::print_1(std::ostringstream &builder,
                                     std::vector<bool> &visited) {
  builder << "(";
  in(1)->print_0(builder, visited);
  builder << "+";
  in(2)->print_0(builder, visited);
  builder << ")";
  return builder;
}

Node *AddNode::phiCon(Node *op, bool rotate) {
  Node *lhs = op->in(1);
  Node *rhs = op->in(2);

  // LHS is either a Phi of constants, or another op with Phi of constants
  PhiNode *lphi = pcon(lhs);
  if (rotate && (lphi == nullptr) && lhs->nIns() > 2) {
    // Only valid to rotate constants if both are same associative ops
    if (typeid(*lhs) != typeid(*op))
      return nullptr;
    lphi = pcon(lhs->in(2));
  }
  if (lphi == nullptr)
    return nullptr;
  // RHS is a constant or a Phi of constants
  if (auto *con = dynamic_cast<ConstantNode *>(rhs);
      !con && pcon(rhs) == nullptr)
    return nullptr;
  // If both are Phis, must be same Region

  if (dynamic_cast<PhiNode *>(rhs) && lphi->in(0) != rhs->in(0))
    return nullptr;

  // Note that this is the exact reverse of Phi pulling a common op down
  // to reduce total op-count.  We don't get in an endless push-up
  // push-down peephole cycle because the constants all fold first.
  std::vector<Node *> ns(lphi->nIns());
  ns[0] = lphi->in(0);
  // Push constant up through the phi: x + (phi con0+con0 con1+con1...)
  for (int i = 1; i < ns.size(); i++) {
    auto result = op->copy(lphi->in(i),
                           typeid(*rhs) == typeid(PhiNode) ? rhs->in(i) : rhs);
    ns[i] = result->peephole();
  }
  std::string label = lphi->label_ + (dynamic_cast<PhiNode *>(rhs)
                                          ? dynamic_cast<PhiNode *>(rhs)->label_
                                          : "");
  Node *phi = (new PhiNode(label, ns))->peephole();
  // Rotate needs another op, otherwise just the phi
  return lhs == lphi ? phi : op->copy(lhs->in(1), phi);
}

PhiNode *AddNode::pcon(Node *op) {
  PhiNode *phi = dynamic_cast<PhiNode *>(op);
  return (phi && phi->allCons()) ? phi : nullptr;
}
Type *AddNode::compute() {
  auto i0 = dynamic_cast<TypeInteger *>(in(1)->type_);
  auto i1 = dynamic_cast<TypeInteger *>(in(2)->type_);

  if (i0 && i1) {
    if (i0->isConstant() && i1->isConstant()) {
      return TypeInteger::constant(i0->value() + i1->value());
    }
  }
  return &Type::BOTTOM;
}

Node *AddNode::idealize() {
  Node *lhs = in(1);
  Node *rhs = in(2);

  Type *t1 = lhs->type_;
  Type *t2 = rhs->type_;

  // Already handled by peephole constant folding
  // They should have been already replaced with constant nodes

  assert(!(t1->isConstant() && t2->isConstant()));

  // Add of 0.  We do not check for (0+x) because this will already
  // canonicalize to (x+0)
  auto i = dynamic_cast<TypeInteger *>(t2);
  if (i && i->value() == 0)
    return lhs;
  // Pointer comparison?

  // Add of same to a multiply by 2
  if (lhs == rhs) {
    return new MulNode(
        lhs, (new ConstantNode(TypeInteger::constant(2), Parser::START))
                 ->peephole());
  }
  // Goal: a left-spine set of adds, with constants on the rhs (which then
  // fold).

  // Move non-adds to RHS
  auto i1 = dynamic_cast<AddNode *>(lhs);
  auto i2 = dynamic_cast<AddNode *>(rhs);
  if (!i1 && i2)
    return swap12();
  // Now we might see (add add non) or (add non non) or (add add add) but never
  // (add non add)

  // Do we have  x + (y + z) ?
  // Swap to    (x + y) + z
  // Rotate (add add add) to remove the add on RHS
  if (i2) {
    auto innerNode = new AddNode(lhs, rhs->in(1));
    auto simplifiedNode = innerNode->peephole();
    return new AddNode(simplifiedNode, rhs->in(2));
  }
  // Now we might see (add add non) or (add non non) but never (add non add) nor
  // (add add add)
  if (!i1) {
    return spline_cmp(lhs, rhs) ? swap12() : phiCon(this, true);
  }

  // Now we only see (add add non)

  if (lhs->in(1) == lhs)
    return nullptr;

  // Do we have (x + con1) + con2?
  // Replace with (x + (con1+con2) which then fold the constants
  if (lhs->in(2)->type_->isConstant() && t2->isConstant()) {
    auto lhsFirst = lhs->in(1);
    auto lhsSecond = lhs->in(2);
    auto innerNode = (new AddNode(lhsSecond, rhs))->peephole();
    return new AddNode(lhsFirst, innerNode);
  }

  // Do we have ((x + (phi cons)) + con) ?
  // Do we have ((x + (phi cons)) + (phi cons)) ?
  // Push constant up through the phi: x + (phi con0+con0 con1+con1...)
  Node *phicon = phiCon(this, true);
  if (phicon != nullptr)
    return phicon;

  // Now we sort along the spline via rotates, to gather similar things
  // together.

  // Do we rotate (x + y) + z
  // into         (x + z) + y ?
  if (spline_cmp(lhs->in(2), rhs))
    return new AddNode(((new AddNode(lhs->in(1), rhs))->peephole()),
                       lhs->in(2));
  return nullptr;
}

// Compare two off-spline nodes and decide what order they should be in.
// Do we rotate ((x + hi) + lo) into ((x + lo) + hi) ?
// Generally constants always go right, then Phi-of-constants, then muls, then
// others. Ties with in a category sort by node ID. TRUE if swapping hi and lo.
bool AddNode::spline_cmp(Node *hi, Node *lo) {
  if (lo->type_->isConstant())
    return false;
  if (hi->type_->isConstant())
    return true;
  if (dynamic_cast<PhiNode *>(lo) && lo->allCons())
    return false;
  if (dynamic_cast<PhiNode *>(hi) && hi->allCons())
    return true;

  if (dynamic_cast<PhiNode *>(lo) && !(dynamic_cast<PhiNode *>(hi)))
    return true;
  if (dynamic_cast<PhiNode *>(hi) && !(dynamic_cast<PhiNode *>(lo)))
    return false;

  return lo->nid > hi->nid;
}

Node *AddNode::copy(Node *lhs, Node *rhs) { return new AddNode(lhs, rhs); }
