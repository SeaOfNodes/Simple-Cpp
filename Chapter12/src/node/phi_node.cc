#include "../../Include/node/phi_node.h"
#include "../../Include/type/type_mem.h"
#include "../../Include/node/cast_node.h"
#include "../../Include/node/if_node.h"
#include "../../Include/node/cproj_node.h"

PhiNode::PhiNode(std::string label, Type* type_, std::initializer_list<Node *> inputs)
    : Node(inputs), declaredType(type_), label_(label) {
}
std::string PhiNode::label() { return "Phi_" + label_; }
std::string PhiNode::glabel() { return "&phi;_" + label_; }

std::ostringstream &PhiNode::print_1(std::ostringstream &builder,
                                     Tomi::Vector<bool> &visited) {
  if (auto *r = dynamic_cast<RegionNode *>(region()); !r || r->inProgress())
    builder << "Z";

  builder << "Phi(";
  for (Node *in : inputs) {
    if (in == nullptr)
      builder << "___";
    else
      in->print_0(builder, visited);
    if (in != inputs.back())
      builder << ",";
  }
  builder << ")";
  return builder;
}

bool PhiNode::isPinned() {
    return true;
}
CFGNode* PhiNode::region() { return (CFGNode*)in(0); }
Type *PhiNode::compute() {
  auto *r = dynamic_cast<RegionNode *>(region());
  if(!r) {
      if(region()->type_ == Type::XCONTROL()) {
          return Type::TOP();
      }
      return type_;
  }
  if(r->inProgress()) return declaredType;
 // Set type to local top of the starting type
  Type *t = declaredType->glb()->dual();
  for (int i = 1; i < nIns(); i++) {
      // If the region's control input is live, add this as a dependency
      // to the control because we can be peeped should it become dead.
    if ((r->in(i)->addDep(this)->type_ != Type::XCONTROL()))
      t = t->meet(in(i)->type_);
  }
  return t;
}

Node *PhiNode::singleUniqueInput() {
  if (auto *loop = dynamic_cast<LoopNode *>(region());
      loop && (loop->entry()->type_ == Type::XCONTROL()))
    return nullptr;

  Node *live = nullptr;
  for (int i = 1; i < nIns(); i++) {
    if (region()->in(i)->addDep(this)->type_ != Type::XCONTROL() &&
        in(i) != this) {
      if (live == nullptr || live == in(i)) {
        live = in(i);
      } else {
        return nullptr;
      }
    }
  }
  return live;
}
bool PhiNode::isMem() {
    auto* a = dynamic_cast<TypeMem*>(declaredType);
    return a != nullptr;
}
bool PhiNode::isMultiTail() { return true; }
Node *PhiNode::idealize() {
  auto *r = dynamic_cast<RegionNode *>(region());
  if (!r)
    return in(1);

  if (r->inProgress() || r->nIns() <= 1)
    return nullptr;

  // Remove a "junk" Phi: Phi(x,x) is just x
  Node *live = singleUniqueInput();
  if (live != nullptr)
    return live;

  /*  Node *op = in(1); // (arg+2)
    std::ostringstream b;
    std::cout << op->print_1(b).str() << "\n";
    std::cout << op->nIns() << "\n";
    std::cout << same_op() << "\n";*/

  // Pull "down" a common data op.  One less op in the world.  One more
  // Phi, but Phis do not make code.
  //   Phi(op(A,B),op(Q,R),op(X,Y)) becomes
  //   op(Phi(A,Q,X), Phi(B,R,Y)).

  Node *op = in(1);
  std::string label = op->label();
  if (op->nIns() == 3 && op->in(0) == nullptr && !op->isCFG() && same_op()) {
    Tomi::Vector<Node *> lhss(nIns());
    Tomi::Vector<Node *> rhss(nIns());
    lhss[0] = rhss[0] = in(0);
    // arg + 1, arg + 2
    // lhss - left hand side operands(region, arg, arg)
    // rhss - right hand side operands(region, 1, 2)
    for (int i = 1; i < nIns(); i++) {
      lhss[i] = in(i)->in(1);
      rhss[i] = in(i)->in(2);
    }
    Node *phi_lhs = alloc.new_object<PhiNode>(label_, declaredType, lhss);
    Node *phi_rhs = alloc.new_object<PhiNode>(label_, declaredType, rhss);
    // Phi(region, arg, arg)
    // Phi(region, 1, 2)
    // now the first one has the same inputs
    phi_lhs = phi_lhs->peephole();
    phi_rhs = phi_rhs->peephole();
    return op->copy(phi_lhs, phi_rhs);
  }
    // If merging Phi(N, cast(N)) - we are losing the cast JOIN effects, so just remove.
    if(nIns() == 3) {
        if(auto* cast = dynamic_cast<CastNode*>(in(1)); cast && cast->in(1)->addDep(this) == in(2)) {
            return in(2);
        }
        if(auto* cast = dynamic_cast<CastNode*>(in(2)); cast && cast->in(1)->addDep(this) == in(1)) {
            return in(1);
        }
    }
    // If merging a null-checked null and the checked value, just use the value.
    // if( val ) ..; phi(Region,False=0/null,True=val);
    // then replace with plain val.
    if(nIns() == 3) {
        int nullx = -1;
        if(in(1)->type_ == in(1)->type_->makeInit()) nullx = 1;
        if(in(2)->type_ == in(2)->type_->makeInit()) nullx = 2;
        if(nullx != -1) {
            Node* val = in(3-nullx);
            if(auto* iff = dynamic_cast<IfNode*>(region()->idom(this)); iff && iff->pred()->addDep(this) == val) {
                // Must walk the idom on the null side to make sure we hit False.
                CFGNode* idom = (CFGNode*)region()->in(nullx);
                while(idom->nIns() > 0 &&  idom->in(0) != iff) idom = idom->idom();
                if(auto* proj = dynamic_cast<CProjNode*>(idom); proj->idx_ == 1) {
                    return val;
                }
            }
        }
    }
  return nullptr;
}
bool PhiNode::allCons(Node *dep) {
  auto *r = dynamic_cast<RegionNode *>(region());
  if (!r)
    return false;
  addDep(dep);
  if (r->inProgress())
    return false;
  return Node::allCons(dep);
}

PhiNode::PhiNode(std::string label, Type* type_, Tomi::Vector<Node *> inputs)
    : Node(inputs), declaredType(type_), label_(label) {}
bool PhiNode::same_op() {
  for (int i = 2; i < nIns(); i++) {
    if (typeid(*in(1)) != typeid(*in(i)))
      return false;
  }
  return true;
}

bool PhiNode::eq(Node *) { return !inProgress(); }
bool PhiNode::inProgress() { return in(nIns() - 1) == nullptr; }
