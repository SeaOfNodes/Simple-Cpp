#include "../../Include/node/xctrl_node.h"
#include "../../Include/node/cfg_node.h"
#include "../../Include/parser.h"

XCtrlNode::XCtrlNode() : CFGNode({Parser::START}) {}

std::string XCtrlNode::label() {
    return "XCtrl";
}

std::ostringstream &XCtrlNode::print_1(std::ostringstream &builder, Tomi::Vector<bool> &visited) {
    return builder;
}

Type *XCtrlNode::compute() {
    return Type::XCONTROL();
}

Node *XCtrlNode::idealize() {
    return nullptr;
}