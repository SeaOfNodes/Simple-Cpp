#include "../../Include/node/if_node.h"

IfNode::IfNode(Node *ctrl, Node *parent) :MultiNode({ctrl, parent}) {

}
std::string IfNode::label() {
    return "if";
}
std::ostringstream& IfNode::print_1(std::ostringstream& builder) {
    builder << "if( ";
    builder << in(1)->print_1(builder).str();
    builder << " )";
    return builder;
}

bool IfNode::isCFG() {
    return true;
}
Node* IfNode::ctrl() {return in(0);}
Node* IfNode::pred() {return in(1);}

Type *IfNode::compute() {
    return TypeTuple::IF;
}

Node* IfNode::idealize() {
    return nullptr;
}
