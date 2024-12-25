#include "../../Include/node/start_node.h"
#include "../../Include/parser.h"

StartNode::StartNode(std::initializer_list<Type *> args) : MultiNode({}) {
  args_ = alloc.new_object<TypeTuple>(args);
  type_ = args_;
}

Tomi::HashMap<std::string, int>  StartNode::aliasStarts = Tomi::HashMap<std::string, int>();
bool StartNode::isCFG() { return true; }

bool StartNode::blockHead() {
    return true;
}
std::ostringstream &StartNode::print_1(std::ostringstream &builder, Tomi::Vector<bool>& visited) {
  builder << label();
  return builder;
}

void StartNode::addMemProj(TypeStruct *ts, ScopeNode *scope) {
    int len = static_cast<int>(args_->types_.size());
    aliasStarts.put(ts->name_, len);

    // resize the tuple's type array to include all fields of the struct
    int max = len + static_cast<int>(ts->fields_.size());
    args_->types_.resize(max);

    // The new members of the tuple get a mem type with an alias
    for(int alias = len; alias < max; alias++) {
        args_->types_[alias] = TypeMem::make(alias);
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
void StartNode::walkUnreach_(Tomi::BitArray<10> &visited, Tomi::HashSet<CFGNode *> &unreach) {

}
int StartNode::loopDepth() {loopDepth_ = 1; return loopDepth_;}
Type *StartNode::compute() { return args_; }

std::string StartNode::label() { return "Start"; }

Node *StartNode::idealize() { return nullptr; }

bool StartNode::isMultiHead() {return true;}
// No immediate dominator, and idepth==0
CFGNode *StartNode::idom() { return nullptr; }
int StartNode::idepth() { return 0; }