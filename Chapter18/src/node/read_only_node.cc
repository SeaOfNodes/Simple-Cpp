#include "node/read_only_node.h"
#include "../../Include/type/type_mem_ptr.h"


ReadOnlyNode::ReadOnlyNode(Node*n) : Node({nullptr, n}) {}
std::string ReadOnlyNode::label() {return "ReadOnly";}

std::ostringstream &ReadOnlyNode::print_1(std::ostringstream &builder, Tomi::Vector<bool> &visited) {
    builder << "(const)";
    in(1)->print_1(builder, visited);
    return builder;
}

Type* ReadOnlyNode::compute() {
    Type*t = in(1)->type_;
    if(auto* tmp = dynamic_cast<TypeMemPtr*>(t)) {
        return tmp->makeR0();
    }
    return t;
}

Node* ReadOnlyNode::idealize() {
    if(nid == 21) {
        std::cerr << "Hello";
    }
    if(auto* tmp = dynamic_cast<TypeMemPtr*>(in(1)->type_); tmp->isFinal()) {
        return in(1);
    }

    return nullptr;
}