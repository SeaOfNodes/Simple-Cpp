#include "../../Include/node/mem_op_node.h"
#include "../../Include/type/type_mem_ptr.h"

MemOpNode::MemOpNode(std::string name, int alias, Node *memSlice, Node *memPtr, Node *value) : Node(
        {nullptr, memSlice, memPtr, value}), name_(name), alias_(alias) {}

Node *MemOpNode::mem() { return in(1); }

Node *MemOpNode::ptr() { return in(2); }

bool MemOpNode::eq(Node *n) {
    MemOpNode *mem = dynamic_cast<MemOpNode *>(n);
    return alias_ == mem->alias_;
}

int MemOpNode::hash() {
    return alias_;
}

std::string MemOpNode::err() {
    Type *ptr1 = ptr()->type_;
    if (ptr1 == Type::BOTTOM()) {
        if (auto *tmp = dynamic_cast<TypeMemPtr *>(ptr1); tmp->nil_) {
            return "Might be null accessing '" + name_ + "'";
        }

    }
    return "";
}