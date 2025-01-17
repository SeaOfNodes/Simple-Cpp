#include "../../Include/node/new_node.h"
#include "../Include/type/type_mem.h"
#include "../../Include/type/tuple_type.h"
#include "../../Include/node/proj_node.h"


NewNode::NewNode(TypeMemPtr* ptr, std::initializer_list<Node*> nodes) : Node(nodes), ptr_(ptr), len_(static_cast<int>(ptr->obj_->fields_.value().size())) {}
NewNode::NewNode(TypeMemPtr* ptr, Tomi::Vector<Node*> nodes) : Node(std::move(nodes)), ptr_(ptr), len_(static_cast<int>(ptr->obj_->fields_.value().size())) {}
std::string NewNode::label() {return "new" + (ptr_->obj_->isAry() ? "ary_" + ptr_->obj_->fields_.value()[1]->type_->str() : ptr_->obj_->str());}
std::string NewNode::glabel() {return "new " + ptr_->obj_->name_;}

ProjNode *NewNode::proj(int idx) {
    for (Node *out: outputs) {
        if (auto *prj = dynamic_cast<ProjNode *>(out); prj->idx_ == idx) {
            return prj;
        }
    }
    return nullptr;
}

std::ostringstream &NewNode::print_1(std::ostringstream &builder, Tomi::Vector<bool>& visited) {
 builder << "new " << ptr_->obj_->name_;
 return builder;
}
Type* NewNode::compute() {
    if(nid == 19) {
        std::cerr << "NewNode::compute() nid == 19" << std::endl;
    }
    Tomi::Vector<Field*> fs = ptr_->obj_->fields_.value();
    Tomi::Vector<Type*> ts(fs.size()+2);
    ts[0] = Type::CONTROL();
    ts[1] = ptr_;
    for(int i = 0; i < fs.size(); i++) {
        auto* mem = dynamic_cast<TypeMem*>(in(i + 2)->type_);
        Type*tfld = in(2+len_ + i)->type_->meet(mem->t_);
        ts[i+2] =  TypeMem::make(fs[i]->alias_, tfld);
    }
    return  TypeTuple::make(ts);
}

int NewNode::find_alias(int alias) {
    return 2 + ptr_->obj_->findAlias(alias) + len_;
}
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