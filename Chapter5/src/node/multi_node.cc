#include "../../Include/node/mutli_node.h"

MultiNode::MultiNode(std::initializer_list<Node *> nodes) : Node(nodes) {
}
std::string MultiNode::label() {
    return "MultiNode";
}
std::ostringstream &MultiNode::print_1(std::ostringstream &builder) {
    return builder;
}

