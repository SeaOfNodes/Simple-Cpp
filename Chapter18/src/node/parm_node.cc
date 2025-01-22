#include "../../Include/node/parm_node.h"
#include "../../Include/node/fun_node.h"

ParmNode::ParmNode(std::string label, int idx, Type* declaredType, std::initializer_list<Node*> inputs) : PhiNode(label, declaredType, inputs), idx_(idx) {}
ParmNode::ParmNode(std::string label, int idx, Type *declaredType, Tomi::Vector<Node *>& inputs) : PhiNode(label, declaredType, inputs){}

std::string ParmNode::label() {
    return MemOpNode::mlabel(label_);
}

std::string ParmNode::glabel() {
    return label_;
}
FunNode* ParmNode::fun() {
    return dynamic_cast<FunNode*>(in(0));
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
    return fun()->inProgress();
}