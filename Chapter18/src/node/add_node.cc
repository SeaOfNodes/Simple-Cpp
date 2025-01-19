#include "../../Include/node/add_node.h"
#include "../../Include/parser.h"
#include "../../Include/node/addf_node.h"

AddNode::AddNode(Node *lhs, Node *rhs) : Node({nullptr, lhs, rhs}) {}

std::string AddNode::label() { return "Add"; }

std::string AddNode::glabel() { return "+"; }

std::ostringstream &AddNode::print_1(std::ostringstream &builder,
                                     Tomi::Vector<bool> &visited) {
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
    if (rhs->type_ == TypeInteger::TOP()) return nullptr;

    // LHS is either a Phi of constants, or another op with Phi of constants
    PhiNode *lphi = pcon(lhs, op);
    if (rotate && (lphi == nullptr) && lhs->nIns() > 2) {
        // Only valid to rotate constants if both are same associative ops
        if (typeid(*lhs) != typeid(*op))
            return nullptr;
        lphi = pcon(lhs->in(2), op);
    }
    if (lphi == nullptr)
        return nullptr;
    // RHS is a constant or a Phi of constants
    if (auto *con = dynamic_cast<ConstantNode *>(rhs);
            !con && pcon(rhs, op) == nullptr)
        return nullptr;
    // If both are Phis, must be same Region

    if (dynamic_cast<PhiNode *>(rhs) && lphi->in(0) != rhs->in(0))
        return nullptr;

    // Note that this is the exact reverse of Phi pulling a common op down
    // to reduce total op-count.  We don't get in an endless push-up
    // push-down peephole cycle because the constants all fold first.
    Tomi::Vector<Node *> ns(lphi->nIns());
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
    Node *phi = alloc.new_object<PhiNode>(label, lphi->declaredType, ns);
    phi = phi->peephole();
    // Rotate needs another op, otherwise just the phi
    return lhs == lphi ? phi : op->copy(lhs->in(1), phi);
}

PhiNode *AddNode::pcon(Node *op, Node *dep) {
    auto *phi = dynamic_cast<PhiNode *>(op);
    return (phi && phi->allCons(dep)) ? phi : nullptr;
}

Type *AddNode::compute() {
    Type *t1 = in(1)->type_;
    Type *t2 = in(2)->type_;

    if (t1->isHigh() || t2->isHigh()) return TypeInteger::TOP();

    auto i1 = dynamic_cast<TypeInteger *>(t1);
    auto i2 = dynamic_cast<TypeInteger *>(t2);


    if (i1 && i2) {
        if (i1->isConstant() && i2->isConstant()) {
            return TypeInteger::constant(i1->value() + i2->value());
        }
        // Fold ranges like {0-1} + {2-3} into {2-4}.
        if(!overflow(i1->min_, i2->min_) && !overflow(i1->max_, i2->max_)) {
            return TypeInteger::make(i1->min_ + i2->min_, i1->max_ + i2->max_);
        }
    }
    return TypeInteger::BOT();
}

bool AddNode::overflow(long x, long y) {
    if((x ^  y) < 0) return false;  // unequal signs, never overflow
    return (x ^ (x + y)) < 0; // sum has unequal signs, so overflow
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
    auto* i = dynamic_cast<TypeInteger *>(t2);
    if (t2 == TypeInteger::ZERO())
        return lhs;
    // Pointer comparison?

    // Add of same to a multiply by 2
    if (lhs == rhs) {
        return alloc.new_object<MulNode>(
                lhs, (alloc.new_object<ConstantNode>(TypeInteger::constant(2), Parser::START))
                        ->peephole());
    }
    // Goal: a left-spine set of adds, with constants on the rhs (which then
    // fold).

    // Move non-adds to RHS
    auto i1 = dynamic_cast<AddNode *>(lhs);
    auto i2 = dynamic_cast<AddNode *>(rhs);
    if (!i1 && i2)
        return swap12();

    // x(-y) becomes x - y
    if (auto *minus = dynamic_cast<MinusNode *>(rhs)) {
        return alloc.new_object<SubNode>(lhs, minus->in(1));
    }
    // Now we might see (add add non) or (add non non) or (add add add) but never
    // (add non add)

    // Do we have  x + (y + z) ?
    // Swap to    (x + y) + z
    // Rotate (add add add) to remove the add on RHS
    if (i2) {
        auto innerNode = alloc.new_object<AddNode>(lhs, rhs->in(1));
        auto simplifiedNode = innerNode->peephole();
        return alloc.new_object<AddNode>(simplifiedNode, rhs->in(2));
    }
    // Now we might see (add add non) or (add non non) but never (add non add) nor
    // (add add add)
    if (!i1) {
        bool spine = spine_cmp(lhs, rhs, this);
        return spine ? swap12() : phiCon(this, false);

        return spine_cmp(lhs, rhs, this) ? swap12() : phiCon(this, true);
    }

    // Now we only see (add add non)

    if (lhs->in(1) == lhs)
        return nullptr;

    // Do we have (x + con1) + con2?
    // Replace with (x + (con1+con2) which then fold the constants
    // lhs.in(2) is con1 here
    // If lhs.in(2) is not a constant, we add ourselves as a dependency
    // because if it later became a constant then we could make this
    // transformation.
    if (lhs->in(2)->addDep(this)->type_->isConstant() && t2->isConstant()) {
        auto lhsFirst = lhs->in(1);
        auto lhsSecond = lhs->in(2);
        auto innerNode = (alloc.new_object<AddNode>(lhsSecond, rhs))->peephole();
        return alloc.new_object<AddNode>(lhsFirst, innerNode);
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
    if (spine_cmp(lhs->in(2), rhs, this)) {
        return alloc.new_object<AddNode>(((alloc.new_object<AddNode>(lhs->in(1), rhs))->peephole()),
                                         lhs->in(2));
    }
    return nullptr;
}

// Compare two off-spline nodes and decide what order they should be in.
// Do we rotate ((x + hi) + lo) into ((x + lo) + hi) ?
// Generally constants always go right, then Phi-of-constants, then muls, then
// others. Ties with in a category sort by node ID. TRUE if swapping hi and lo.
bool AddNode::spine_cmp(Node *hi, Node *lo, Node *dep) {
    if (lo->type_->isConstant())
        return false;
    if (hi->type_->isConstant())
        return true;

    if (auto lphi = dynamic_cast<PhiNode *>(lo);
            lphi && lphi->region()->type_ == Type::XCONTROL())
        return false;
    if (auto hphi = dynamic_cast<PhiNode *>(hi);
            hphi && hphi->region()->type_ == Type::XCONTROL())
        return false;

    if (dynamic_cast<PhiNode *>(lo) && lo->allCons(dep))
        return false;
    if (dynamic_cast<PhiNode *>(hi) && hi->allCons(dep))
        return true;

    if (dynamic_cast<PhiNode *>(lo) && !(dynamic_cast<PhiNode *>(hi)))
        return true;
    if (dynamic_cast<PhiNode *>(hi) && !(dynamic_cast<PhiNode *>(lo)))
        return false;

    // Same category of "others"
    return lo->nid > hi->nid;
}

Node *AddNode::copy(Node *lhs, Node *rhs) { return alloc.new_object<AddFNode>(lhs, rhs); }

Node *AddNode::copyF() {
    return alloc.new_object<AddFNode>(nullptr, nullptr);
}