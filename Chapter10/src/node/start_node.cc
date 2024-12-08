#include "../../Include/node/start_node.h"
#include "../../Include/parser.h"

StartNode::StartNode(std::initializer_list<Type *> args) : MultiNode({}) {
  args_ = alloc.new_object<TypeTuple>(args);
  type_ = args_;
}

bool StartNode::isCFG() { return true; }

std::ostringstream &StartNode::print_1(std::ostringstream &builder, Tomi::Vector<bool>& visited) {
  builder << label();
  return builder;
}

void StartNode::addMemProj(TypeStruct *ts, ScopeNode *scope) {
    int len = args_->types_.size();

    // resize the tuple's type array to include all fields of the struct
    int max = len + ts->fields_.size();
    args_->types_.resize(max);

    // The new members of the tuple get a mem type with an alias
    for(int alias = len; alias < max; alias++) {
        args_->types_[alias] = alloc.new_object<TypeMem>(alias);
    }
    type_ = args_ = TypeTuple::make(args_->types_);
    // For each of the fields we now add a mem projection.  Note that the
    // alias matches the slot of the field in the tuple
    for(int alias = len; alias < max; alias++) {
        std::string name = Parser::memName(alias);
        Node* n = (new ProjNode(this, alias, name))->peephole();
        scope->define(name, args_->types_[alias], n);
    }
}
Type *StartNode::compute() { return args_; }

std::string StartNode::label() { return "Start"; }

Node *StartNode::idealize() { return nullptr; }

bool StartNode::isMultiHead() {return true;}
// No immediate dominator, and idepth==0
Node *StartNode::idom() { return nullptr; }