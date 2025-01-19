#include "../../Include/node/ctrl_node.h"
#include "../../Include/parser.h"
CtrlNode::CtrlNode() : CFGNode(Parser::START()) {
}

std::string CtrlNode::label() {
    return "Ctrl";
}

std::ostringstream CtrlNode::print_1(std::ostringstream& os, Tomi::BitArray<10>& visited) {
    os << "Cctrl";
    return os;
}

bool CtrlNode::isConst() {
    return true;
}

Type* CtrlNode::compute() {
    return Type::CONTROL;
}

Node* CtrlNode::idealize() {
    return nullptr;
}