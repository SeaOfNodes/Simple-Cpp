#include "../../Include/node/add_node.h"

#include <node/constant_node.h>
#include <node/mul_node.h>

#include "../../Include/type/integer_type.h"

AddNode::AddNode(Node *lhs, Node *rhs) : Node({nullptr, lhs, rhs}) {}

std::string AddNode::label() { return "Add"; }
std::string AddNode::glabel() { return "+"; }

std::ostringstream &AddNode::print_1(std::ostringstream &builder) {
  builder << "(";
  in(1)->print_1(builder);
  builder << "+";
  in(2)->print_1(builder);
  builder << ")";
  return builder;
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
    return spline_cmp(lhs, rhs) ? swap12() : nullptr;
  }

  // Now we only see (add add non)
  // Do we have (x + con1) + con2?
  // Replace with (x + (con1+con2) which then fold the constants
  if (lhs->in(2)->type_->isConstant() && t2->isConstant()) {
    auto lhsFirst = lhs->in(1);
    auto lhsSecond = lhs->in(2);
    auto innerNode = (new AddNode(lhsSecond, rhs))->peephole();
    return new AddNode(lhsFirst, innerNode);
  }

  // Now we sort along the spline via rotates, to gather similar things
  // together.

  // Do we rotate (x + y) + z
  // into         (x + z) + y ?
  if (spline_cmp(lhs->in(2), rhs)) {
    Node *firstInput = lhs->in(1);
    Node *secondInput = lhs->in(2);
    Node *innerNode = (new AddNode(firstInput, rhs))->peephole();
    return new AddNode(innerNode, secondInput);
  }
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
    return false;
  if (dynamic_cast<PhiNode *>(hi) && !(dynamic_cast<PhiNode *>(lo)))
    return false;

  return lo->nid > hi->nid;
}

Node *AddNode::copy(Node *lhs, Node *rhs) { return new AddNode(lhs, rhs); }
