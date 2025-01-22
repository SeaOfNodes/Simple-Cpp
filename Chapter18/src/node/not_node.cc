#include "../../Include/node/not_node.h"
#include "../../Include/type/integer_type.h"
#include "../../Include/type/type_mem_ptr.h"
#include "../../Include/type/type_float.h"

NotNode::NotNode(Node *in) : Node({nullptr, in}) {
}

std::string NotNode::label() { return "Not"; }

std::string NotNode::glabel() { return "!"; }

std::ostringstream &NotNode::print_1(std::ostringstream &builder, Tomi::Vector<bool> &visited) {
    builder << "(!";
    in(1)->print_1(builder, visited);
    builder << ")";
    return builder;
}

Type *NotNode::compute() {
    Type *t0 = in(1)->type_;

    if (in(1)->type_->isHigh()) return TypeInteger::BOOL()->dual();
    if (t0->isHigh()) return TypeInteger::BOOL()->dual();
    if (t0 == Type::NIL() || t0 == TypeInteger::ZERO()) return TypeInteger::TRUE();
    auto *tn = dynamic_cast<TypeMemPtr *>(t0);
    if (tn && tn->notNull()) return TypeInteger::FALSE();
    auto *ti = dynamic_cast<TypeInteger *>(t0);
    if (ti && (ti->min_ > 0 || ti->max_ < 0)) return TypeInteger::FALSE();
    return TypeInteger::BOOL();
}

Node *NotNode::idealize() { return nullptr; }
