#include "../../Include/node/start_node.h"
StartNode::StartNode(std::initializer_list<Type *> args) : MultiNode({}) {
  args_ = new TypeTuple({args});
  type_ = args_;
}

bool StartNode::isCFG() { return true; }

std::ostringstream &StartNode::print_1(std::ostringstream &builder) {
  builder << label();
  return builder;
}

Type *StartNode::compute() { return args_; }

std::string StartNode::label() { return "Start"; }

Node *StartNode::idealize() { return nullptr; }
