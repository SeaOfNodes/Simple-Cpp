#include "../../Include/node/load_node.h"

LoadNode::LoadNode(std::string name, int alias, Type *glb, Node *memSlice, Node *memPtr) : MemOpNode(name, alias, memSlice, memPtr, nullptr), declaredType(glb) {}

std::string LoadNode::label() {return "Load";}
std::string LoadNode::glabel() {return "."+name_;}
std::ostringstream &LoadNode::print_1(std::ostringstream &builder, Tomi::Vector<bool> &visited) {
   builder << "." << name_;
    return builder;
}

Type *LoadNode::compute() {
    return declaredType;
}

// Todo: finish these
Node* LoadNode::idealize() {
    // Simple Load-after-Store on same address.

}
bool LoadNode::profit(PhiNode* phi, int idx) {
    Node* px = phi->in(idx);

}