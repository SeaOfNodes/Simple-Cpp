#include "../../Include/node/not_node.h"
#include "../../Include/type/integer_type.h"

NotNode::NotNode(Node *in) : Node({nullptr, in}) {
}

std::string NotNode::label() { return "Not"; }

std::string NotNode::glabel() { return "!"; }

std::ostringstream &NotNode::print_1(std::ostringstream &builder, Tomi::Vector<bool>& visited) {
    builder << "(!";

    builder << in(1)->print_1(builder, visited).str();
    builder << ")";
    return builder;
}

Type *NotNode::compute() {
    if (auto *i = dynamic_cast<TypeInteger *>(in(1)->type_)) {
        return i->isConstant() ? TypeInteger::constant(i->value() == 0 ? 1 : 0) : i;
    }
    return TypeInteger::TOP()->meet(in(1)->type_);
}

Node *NotNode::idealize() { return nullptr; }
