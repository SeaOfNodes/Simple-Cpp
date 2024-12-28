#include "../../Include/node/new_node.h"

NewNode::NewNode(TypeMemPtr* ptr, Node* ctrl) : Node({ctrl}), ptr_(ptr) {}

std::string NewNode::label() {return "new";}
std::string NewNode::glabel() {return "new " + ptr_->obj_->name_;}

std::ostringstream &NewNode::print_1(std::ostringstream &builder, Tomi::Vector<bool>& visited) {
 builder << "new " << ptr_->obj_->name_;
 return builder;
}
Type* NewNode::compute() {return ptr_;}

bool NewNode::isPinned() {
    return true;
}
Node* NewNode::idealize() {return nullptr;}

bool NewNode::eq(Node* n) {
return this == n;
}
int NewNode::hash() {
    return ptr_->hashCode();
}