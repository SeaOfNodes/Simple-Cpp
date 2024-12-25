#include "../../Include/node/minusf_node.h"

MinusFNode::MinusFNode(Node *in) : Node({nullptr, in}) {
}

std::string MinusFNode::label() {
    return "MinusF";
}

std::string MinusFNode::glabel() {
    return "-";
}

std::ostringstream &MinusFNode::print_1(std::ostringstream &builder, Tomi::Vector<bool> &visited) {
    builder << "-("
    in(1)->print(builder);
    builder << ")";
    return builder;
}

Type *MinusFNode::compute() {
    if (auto *i0 = dynamic_cast<TypeFloat *>(in(1)->type_)) {
        if (i0.isConstant()) {
            return TypeFloat::constant(-i0.value());
        }
        return i0;
    }
    return TypeFloat::BOT();
}

Node *MinusFNode::idealize() {
    // -(-x) is x
    if(dynamic_cast<MinusFNode*>((1)) {
        return minus->in(1);
    }
    return nullptr;
}