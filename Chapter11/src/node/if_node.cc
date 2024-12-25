#include "../../Include/node/if_node.h"
#include "../../Include/parser.h"
#include "../../Include/node/cproj_node.h"
IfNode::IfNode(Node *ctrl, Node *parent) : MultiNode({ctrl, parent}) {
    IterPeeps::add(this);
}
std::string IfNode::label() { return "if"; }
std::ostringstream &IfNode::print_1(std::ostringstream &builder,
                                    Tomi::Vector<bool> &visited) {
  builder << "if( ";
  builder << in(1)->print_1(builder, visited).str();
  builder << " )";
  return builder;
}

bool IfNode::isCFG() { return true; }
Node *IfNode::ctrl() { return in(0); }
Node *IfNode::pred() { return in(1); }

bool IfNode::isMultiHead() { return true; }

Type *IfNode::compute() {
  // If the If node is not reachable then neither is any following Proj
  if (ctrl()->type_ != Type::CONTROL() && ctrl()->type_ != Type::BOTTOM())
    return TypeTuple::IF_NEITHER();
  Node *predc = pred();
  Type *t = predc->type_;

  // High types mean NEITHER side is reachable.
  // Wait until the type falls to decide which way to go.

  if (t == Type::TOP() || t == TypeInteger::TOP())
    return TypeTuple::IF_NEITHER();

  // If constant is 0 then false branch is reachable
  // Else true branch is reachable
  if (auto *ti = dynamic_cast<TypeInteger *>(pred()->type_); ti) {
    if (ti->isConstant()) {
      // Your code here
      if (ti == TypeInteger::ZERO())
        return TypeTuple::IF_FALSE();
      else
        return TypeTuple::IF_TRUE();
    }
  }

  return TypeTuple::IF_BOTH();
}

Node *IfNode::idealize() {
  if (!pred()->type_->isHighOrConst()) {
    CFGNode *dom = idom();
    CFGNode *prior = this;

    while (dom != nullptr) {
      Node* result = dom->addDep(this);
      auto *iff = dynamic_cast<IfNode *>(result);
      auto *prj = dynamic_cast<ProjNode *>(prior);

      if (iff && iff->pred()->addDep(this) == pred() && prj) {
        int value = (prj->idx_ == 0) ? 1 : 0;
        setDef(1,
               (alloc.new_object<ConstantNode>(TypeInteger::make(true, value), Parser::START))
                   ->peephole());
        return this;
      }

      prior = dom;
      dom = dom->idom();
    }
  }
  return nullptr;
}

void IfNode::walkUnreach_(Tomi::BitArray<10> &visited, Tomi::HashSet<CFGNode *>& unreach) {
    for(Node* proj: outputs) {
        if(dynamic_cast<CProjNode*>(proj)->loopDepth_ == 0) {
            unreach.put((CProjNode*)proj);
        }
    }
    CFGNode::walkUnreach_(visited, unreach);
}

Node* IfNode::getBlockStart() {
   return ctrl()->getBlockStart();
}