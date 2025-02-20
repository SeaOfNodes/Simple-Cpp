#include "../../Include/node/ctrl_node.h"
#include "../../Include/parser.h"

CtrlNode::CtrlNode() : CFGNode(std::initializer_list<Node*>{Parser::START}) {
}

std::string CtrlNode::label() {
    return "Ctrl";
}

std::ostringstream &CtrlNode::print_1(std::ostringstream &os, Tomi::Vector<bool> &visited) {
    os << "Cctrl";
    return os;
}

bool CtrlNode::isConst() {
    return true;
}

Type *CtrlNode::compute() {
    return Type::CONTROL();
}

Node *CtrlNode::idealize() {
    return nullptr;
}