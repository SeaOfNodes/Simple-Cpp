#include "../../Include/node/never_node.h"

NeverNode::NeverNode(Node *ctrl) : IfNode(ctrl, nullptr) {}
std::string NeverNode::label() { return "Never"; }
std::ostringstream &NeverNode::print_1(std::ostringstream &builder, Tomi::Vector<bool> &visited) {
    builder << "Never";
    return builder;
}
Type *NeverNode::compute() { return TypeInteger::BOT(); }
Node *NeverNode::idealize() { return nullptr; }
