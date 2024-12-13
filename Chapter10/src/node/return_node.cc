#include "../../Include/node/return_node.h"

#include "../../Include/type/tuple_type.h"

ReturnNode::ReturnNode(Node *ctrl, Node *data, ScopeNode* scope) : Node({ctrl, data}) {
    // We lookup memory slices by the naming convention that they start with $
    // We could also use implicit knowledge that all memory projects are at offset >= 2
    Tomi::Vector<std::string> names = scope->reverseNames();
    for(std::string name: names) {
        if(name != "$ctrl" && name[0] == '$') {
            addDef(scope->lookup(name));
        }
    }
}

[[nodiscard]] Node *ReturnNode::ctrl() { return in(0); }
[[nodiscard]] Node *ReturnNode::expr() { return in(1); }

bool ReturnNode::isCFG() { return true; }

std::ostringstream &ReturnNode::print_1(std::ostringstream &builder,
                                        Tomi::Vector<bool> &visited) {
  builder << "return ";
  Node *expr1 = expr();
  expr1->print_0(builder, visited);
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
