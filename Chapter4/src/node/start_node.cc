#include "../../Include/node/start_node.h"

#include <complex>


StartNode::StartNode(std::initializer_list<Type *> args) {
    args_ = new TypeTuple({args});
    type_ = args_;
}

bool StartNode::isCFG() const { return true; }

std::ostringstream &StartNode::print_1(std::ostringstream &builder) {
    builder << label();
    return builder;
}

Type *StartNode::compute() {
    return args_;
}

std::string StartNode::label() {
    return "Start";
}

Node *StartNode::idealize() { return nullptr; }
