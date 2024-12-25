#include "../../Include/node/cast_node.h"

CastNode::CastNode(Type* t_, Node* ctrl, Node* in) : Node({ctrl, in}), t(t_) {
    setType(CastNode::compute());
}
std::string CastNode::label() {
    return "(" + t->str() + ")";
}
std::string CastNode::uniqueName() {
   return "Cast_" + std::to_string(nid);
}
std::ostringstream& CastNode::print_1(std::ostringstream& builder, Tomi::Vector<bool>& visited) {
    builder << label() << in(0)->print_1(builder, visited).str();
    return builder;
}

Type* CastNode::compute() {
    return in(1)->type_->join(t);
}
Node* CastNode::idealize() {
   return in(1)->type_->isa(t) ? in(1) : nullptr;
}