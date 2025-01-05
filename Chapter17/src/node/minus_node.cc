#include "../../Include/node/minus_node.h"
#include "../../Include/type/integer_type.h"
#include "../../Include/node/minusf_node.h"
#include <limits>

MinusNode::MinusNode(Node *in) : Node({nullptr, in}) {}

std::string MinusNode::label() { return "Minus"; }
std::string MinusNode::glabel() { return "-"; }

std::ostringstream &MinusNode::print_1(std::ostringstream &builder,
                                       Tomi::Vector<bool> &visited) {
  builder << "(-";
  in(1)->print_0(builder, visited);
  builder << ")";
  return builder;
}

Type *MinusNode::compute() {
  auto i0 = dynamic_cast<TypeInteger *>(in(1)->type_);
  if(i0) {
      if (i0->isHigh()) return TypeInteger::TOP();
      // Catch overflow wrapping on `-Long.MIN_VALUE`
        if(i0 == TypeInteger::BOT() || i0->min_ == std::numeric_limits<long>::min() || i0->max_ == std::numeric_limits<long>::max()) {
            return TypeInteger::TOP();
        }
      return TypeInteger::make(-i0->max_, -i0->min_);
  }
  return TypeInteger::TOP()->meet(in(1)->type_);
}

Node *MinusNode::idealize() {
    // -(-x) is x
    if(auto* minus = dynamic_cast<MinusNode*>(in(1))) {
        return minus->in(1);
    }
    return nullptr;
}

Node* MinusNode::copyF() {
return alloc.new_object<MinusFNode>(nullptr);
}