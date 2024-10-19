#include "../../Include/node/if_node.h"

IfNode::IfNode(Node *ctrl, Node *parent) : MultiNode({ctrl, parent}) {}
std::string IfNode::label() { return "if"; }
std::ostringstream &IfNode::print_1(std::ostringstream &builder,
                                    std::vector<bool> visited) {
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
  if (ctrl()->type_ != &Type::CONTROL && ctrl()->type_ != &Type::BOTTOM)
    return &TypeTuple::IF_NEITHER;
  // If constant is 0 then false branch is reachable
  // Else true branch is reachable
  if (TypeInteger *ti = dynamic_cast<TypeInteger *>(pred()->type_)) {
    if (ti->isConstant()) {
      // Your code here
      if (ti->value() == 0)
        return &TypeTuple::IF_FALSE;
      else
        return &TypeTuple::IF_TRUE;
    }
  }

  // Hunt up the immediate dominator tree.  If we find an identical if
  // test on either the true or false branch, then this test matches.
  Node *dom = idom();
  Node *prior = this;

  while (dom != nullptr) {
    prior = dom;
    dom = dom->idom();
    if (auto *iff = dynamic_cast<IfNode *>(dom); iff && iff->pred() == pred()) {
      ProjNode *proj = dynamic_cast<ProjNode *>(prior);

      if (prior) {
        return (proj->idx_ == 0) ? &TypeTuple::IF_TRUE : &TypeTuple::IF_FALSE;
      } else {
        return nullptr;
      }
    }
  }
  return &TypeTuple::IF_BOTH;
}

Node *IfNode::idealize() { return nullptr; }
