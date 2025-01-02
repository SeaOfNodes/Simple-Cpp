#include "../../Include/node/return_node.h"

#include "../../Include/type/tuple_type.h"

ReturnNode::ReturnNode(Node *ctrl, Node *data, ScopeNode* scope) : CFGNode({ctrl, data}) {
    // We lookup memory slices by the naming convention that they start with $
    // We could also use implicit knowledge that all memory projects are at offset >= 2
    // Add memory slices to Return, so all memory updates are live-on-exit.
    if(scope != nullptr) {
    Tomi::Vector<std::string> names = scope->reverseNames();
    for(std::string name: names) {
        if(name != "$ctrl" && name[0] == '$') {
            addDef(scope->lookup(name));
        }
    }
    }
}

[[nodiscard]] Node *ReturnNode::ctrl() { return in(0); }
[[nodiscard]] Node *ReturnNode::expr() { return in(1); }

bool ReturnNode::isCFG() { return true; }

Node* ReturnNode::getBlockStart() {
  return ctrl()->getBlockStart();
}
std::ostringstream &ReturnNode::print_1(std::ostringstream &builder,
                                        Tomi::Vector<bool> &visited) {
  builder << "return ";
  Node *expr1 = expr();
  std::string b = builder.str();
  expr1->print_0(builder, visited);
  std::string a = builder.str();
  builder << ";";
  return builder;
}

Type *ReturnNode::compute() {
  return TypeTuple::make({ctrl()->type_, expr()->type_});
}

std::string ReturnNode::label() { return "Return"; }

// for some reasons instead of getting ~ctrl im getting #
Node *ReturnNode::idealize() {
  if (ctrl()->type_ == Type::XCONTROL()) {
    return ctrl();
  }
  return nullptr;
}
