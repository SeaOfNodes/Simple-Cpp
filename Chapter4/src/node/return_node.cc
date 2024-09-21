#include "../../Include/node/return_node.h"

ReturnNode::ReturnNode(Node *ctrl, Node *data) : Node({ctrl, data}) {
}

[[nodiscard ]] Node *ReturnNode::ctrl() { return in(0); }
[[nodiscard ]] Node *ReturnNode::expr() { return in(1); }

bool ReturnNode::isCFG() const { return true; }

std::ostringstream &ReturnNode::print_1(std::ostringstream &builder) {
    builder << "return ";
    Node *expr1 = expr();
    expr1->print_1(builder);
    builder << ";";
    return builder;
}

Type *ReturnNode::compute() {
    return &Type::BOTTOM;
}

std::string ReturnNode::label() {
    return "Return";
}

Node *ReturnNode::idealize() {
    return nullptr;
}
