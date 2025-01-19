#include "../../Include/node/sub_node.h"
#include "../../Include/type/integer_type.h"
#include "../../Include/node/minus_node.h"
#include "../../Include/node/add_node.h"

SubNode::SubNode(Node *lhs, Node *rhs) : Node({nullptr, lhs, rhs}) {}

std::string SubNode::label() { return "Sub"; }

std::string SubNode::glabel() { return "-"; }

std::ostringstream &SubNode::print_1(std::ostringstream &builder,
                                     Tomi::Vector<bool> &visited) {
    builder << "(";
    in(1)->print_0(builder, visited);

    builder << "-";
    in(2)->print_0(builder, visited);

    builder << ")";

    return builder;
}

Type *SubNode::compute() {
    auto t1 = dynamic_cast<TypeInteger *>(in(1)->type_);
    auto t2 = dynamic_cast<TypeInteger *>(in(2)->type_);
    if(t1->isHigh() || t2->isHigh()) return TypeInteger::TOP();

    // Sub of same
    if (in(1) == in(2))
        return TypeInteger::ZERO();

    if (t1 && t1->isConstant() && t2 && t2->isConstant()) {
        return TypeInteger::constant(t1->value() - t2->value());
    }

    // Fold ranges like {2-3} - {0-1} into {1-3}.
    if(!AddNode::overflow(t1->min_, -t2->max_) &&
      !AddNode::overflow(t1->max_, -t2->max_) &&
      t2->min_ != std::numeric_limits<long>::min()) {
        return TypeInteger::make(t1->min_ - t2->max_, t1->max_ - t2->min_);
    }


    return TypeInteger::BOT();
}

Node *SubNode::idealize() {
    // x - (-y) is x+y
    if (auto *min = dynamic_cast<MinusNode *>(in(2))) {
        return new AddNode(in(1), min->in(2));
    }
    // (-x) - y is -(x+y)
    if (auto *min = dynamic_cast<MinusNode *>(in(1))) {
        Node*add = alloc.new_object<AddNode>(min->in(1), in(2))->peephole();
        return (alloc.new_object<MinusNode>(add));
    }
    return nullptr;
}

Node *SubNode::copy(Node *lhs, Node *rhs) { return alloc.new_object<SubNode>(lhs, rhs); }

Node *SubNode::copyF() {
    return alloc.new_object<SubNode>(nullptr, nullptr);
}