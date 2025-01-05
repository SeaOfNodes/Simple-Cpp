#include "../../Include/node/start_node.h"
#include "../../Include/parser.h"

StartNode::StartNode(Type* arg) : LoopNode({nullptr}) {
  arg_ = arg;
  type_ = TypeTuple::make({Type::CONTROL(), TypeMem::TOP(), arg_});
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
//    if(ts->fields_.value().empty()) return;
//
//    // Expand args type for more memory projections
//    Tomi::Vector<Type*> args = arg_->types_;
//    int max = static_cast<int>(args.size());
//    for(Field* f: ts->fields_.value()) {
//        max = std::max(max, f->alias_);
//    }
//   args.resize(max + 1);
//    for(Field*f: ts->fields_.value()) {
//        TypeMem* tm_decl = TypeMem::make(f->alias_, f->type_->glb());
//        args[f->alias_] = tm_decl->dual();
//        std::string name = Parser::memName(f->alias_);
//        Node*n = alloc.new_object<ProjNode>(this, f->alias_, name);
//        n->type_ = args[f->alias_];
//        scope->define(name, tm_decl, n);
//    }
//    for(int i = 0; i < args.size(); i++) {
//        if(args[i] == nullptr) args[i] = Type::TOP();
//    }
//    type_ = args_ = TypeTuple::make(args);
}

std::string StartNode::label() { return "Start"; }

Node *StartNode::idealize() { return nullptr; }

bool StartNode::isMultiHead() {return true;}
// No immediate dominator, and idepth==0
CFGNode *StartNode::idom(Node* dep) { return nullptr; }
int StartNode::idepth() { return 0; }