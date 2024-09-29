#include "../../Include/node/if_node.h"

IfNode::IfNode(Node *ctrl, Node *parent) : MultiNode({ctrl, parent}) {}
std::string IfNode::label() { return "if"; }
std::ostringstream &IfNode::print_1(std::ostringstream &builder) {
  builder << "if( ";
  builder << in(1)->print_1(builder).str();
  builder << " )";
  return builder;
}

bool IfNode::isCFG() const { return true; }
Node *IfNode::ctrl() { return in(0); }
Node *IfNode::pred() { return in(1); }

Type *IfNode::compute() {
  // If the If node is not reachable then neither is any following Proj
  if(ctrl()->type_ != &Type::CONTROL) return &TypeTuple::IF_NEITHER;
  // If constant is 0 then false branch is reachable
  // Else true branch is reachable
  if(TypeInteger* ti = dynamic_cast<TypeInteger*>(pred()->type_)) {
    if (ti->isConstant()) {
      // Your code here
      if(ti->value() == 0) return &TypeTuple::IF_FALSE;
      else return &TypeTuple::IF_TRUE;
    }
  }

  // Hunt up the immediate dominator tree.  If we find an identical if
  // test on either the true or false branch, then this test matches.
  return &TypeTuple::IF_BOTH;

}

Node *IfNode::idealize() { return nullptr; }
