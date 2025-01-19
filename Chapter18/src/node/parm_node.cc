#include "../../Include/node/parm_node.h"

ParmNode::ParmNode(std::string label, int idx, Type* declaredType, std::initializer_list<Node*> inputs) : Node(label, declaredType, inputs), idx_(idx) {}
ParmNode::ParmNode(std::string label, int idx, Type *declaredType, Tomi::Vector<Node *>& inputs) {}

std::string ParmNode::label() {
    return MemOpNode::mlabel(label_);
}

std::string ParmNode::glabel() {
    return label_;
}
FunNode* ParmNode::fun() {
    return in(0);
}
std::ostringstream& ParmNode::print_1(std::ostringstream& builder, Tomi::Vector<bool>& visited) {
    if(visited[idx_]) {
        builder << "Parm" << idx_;
    } else {
        visited[idx_] = true;
        in(0)->print(builder);
        visited[idx_] = false;
    }
    return builder;
}

Node *ParmNode::idealize() {
    if(inProgress()) return nullptr;
    return PhiNode::idealize();
}

bool ParmNode::inProgress() {
    return fun(0)->inProgress();
}