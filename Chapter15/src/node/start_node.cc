#include "../../Include/node/start_node.h"
#include "../../Include/parser.h"

StartNode::StartNode(std::initializer_list<Type *> args) : MultiNode({}) {
  args_ = alloc.new_object<TypeTuple>(args);
  type_ = args_;
}

bool StartNode::isCFG() { return true; }

bool StartNode::blockHead() {
    return true;
}

CFGNode* StartNode::cfg0() {
    return this;
}
std::ostringstream &StartNode::print_1(std::ostringstream &builder, Tomi::Vector<bool>& visited) {
  builder << label();
  return builder;
}

void StartNode::addMemProj(TypeStruct *ts, ScopeNode *scope) {
    if(ts->fields_.value().empty()) return;

    // Expand args type for more memory projections
    Tomi::Vector<Type*> args = args_->types_;
    int max = static_cast<int>(args.size());
    for(Field* f: ts->fields_.value()) {
        max = std::max(max, f->alias_);
    }
   args.resize(max + 1);
    for(Field*f: ts->fields_.value()) {
        TypeMem* tm_decl = TypeMem::make(f->alias_, f->type_->glb());
        args[f->alias_] = tm_decl->dual();
        std::string name = Parser::memName(f->alias_);
        Node*n = alloc.new_object<ProjNode>(this, f->alias_, name);
        n->type_ = args[f->alias_];
        scope->define(name, tm_decl, n);
    }
    for(int i = 0; i < args.size(); i++) {
        if(args[i] == nullptr) args[i] = Type::TOP();
    }
    type_ = args_ = TypeTuple::make(args);
}
void StartNode::walkUnreach_(Tomi::BitArray<10> &visited, Tomi::HashSet<CFGNode *> &unreach) {

}
int StartNode::loopDepth() {loopDepth_ = 1; return loopDepth_;}
Type *StartNode::compute() { return args_; }

std::string StartNode::label() { return "Start"; }

Node *StartNode::idealize() { return nullptr; }

bool StartNode::isMultiHead() {return true;}
// No immediate dominator, and idepth==0
CFGNode *StartNode::idom(Node* dep) { return nullptr; }
int StartNode::idepth() { return 0; }