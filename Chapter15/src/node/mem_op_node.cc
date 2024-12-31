#include "../../Include/node/mem_op_node.h"
#include "../../Include/type/type_mem_ptr.h"

// Todo: Bug prone(follow what main implementation does)
MemOpNode::MemOpNode(std::string name, int alias, Type *glb, Node *mem, Node *ptr, Node *off) : Node(
        {nullptr, mem, ptr, off}), name_(name), alias_(alias), declaredType(glb) {}

MemOpNode::MemOpNode(std::string name, int alias, Type *glb, Node *mem, Node *ptr, Node *off, Node *val) : MemOpNode(
        name, alias, glb, mem, ptr, off
) {
    addDef(val);
}

Node *MemOpNode::mem() { return in(1); }

Node *MemOpNode::ptr() { return in(2); }

bool MemOpNode::eq(Node *n) {
    MemOpNode *mem = dynamic_cast<MemOpNode *>(n);
    return alias_ == mem->alias_;
}

Node *MemOpNode::off() {
    return in(3);
}

std::string MemOpNode::mlabel(std::string name) {
    if (name == "[]") return "ary";
    if (name == "#") return "len";
    return name;
}

std::string MemOpNode::mlabel() {
    return mlabel(name_);
}

int MemOpNode::hash() {
    return alias_;
}

std::string MemOpNode::err() {
    Type *ptr1 = ptr()->type_;
    if (ptr1 == Type::BOTTOM()) {
        return "";
    }
    if (auto *tmp = dynamic_cast<TypeMemPtr *>(ptr1); tmp && !tmp->nil_) {
        return "";

    }
    return "Might be null accessing '" + name_ + "'";
}