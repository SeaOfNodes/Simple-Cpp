#include "../../Include/node/struct_node.h"
#include "../../Include/type/type.h"
#include "../../Include/type/type_struct.h"

std::string StructNode::label() {
    if(ts_ == nullptr) {
       return  "STRUCT?";
    } else {
  return  ts_->str();
    }
}

std::ostringstream& StructNode::print_1(std::ostringstream &builder,
Tomi::Vector<bool> &) {
    if(ts_ == nullptr) {
        builder << "STRUCT?";
    }
    builder << ts_->name_;
    builder << " {";
    for(int i = 0; i < nIns(); i++) {
        builder << ts_->fields_.value()[i]->fname_;
        builder << ":";
        if(in(i) == nullptr) {
            Type::BOTTOM()->print_1(builder);
        } else {
            in(i)->print(builder);
        }
    }
    builder << "}";
    return builder;
}

Type* StructNode::compute() {
    if(ts_ == nullptr) {
        return TypeStruct::BOTTOM();
    }
    Tomi::Vector<Field*> fs(ts_->fields_.value().size());
    for(int i = 0; i < fs.size(); i++) {
        fs[i] = ts_->fields_.value()[i]->makeFrom(in(i) == nullptr ? Type::TOP() : in(i)->type_);

    }
    return TypeStruct::make(ts_->name_, fs);
}

Node* StructNode::idealize() {
    return nullptr;
}

bool StructNode::eq(Node* n) {
    return ts_ == dynamic_cast<StructNode*>(n)->ts_;
}

int StructNode::hash() {
    return ts_ == nullptr ? 0: ts_->hash();
}
