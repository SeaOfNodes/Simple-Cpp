#include "../Include/codegen.h"
#include "../Include/parser.h"
#include "../Include/globalCodeMotion.h"
#include "../Include/list_scheduler.h"
#include "../Include/type/type_fun_ptr.h"
#include "../Include/node/fun_node.h"

CodeGen::CodeGen(std::string src, TypeInteger* arg) : phase_(nullptr), src_(src), arg_(arg), CODE(this)  {

}
CodeGen::CodeGen(std::string src) : CodeGen(src, TypeInteger::BOT()) {}

CodeGen *CodeGen::parse() {
    return parse(true);
}

CodeGen::parse(bool disable) {
    phase_ = Phase::Parse;
    linker_.clear();
    p = alloc.new_object<Parser>(src_, arg_);
    stop_ = P.p();
    start_ = Parser::START; // Capture global start
    p = nullptr; // No longer parsing
    return this;
}

CodeGen *CodeGen::opto() {
    phase_ = Phase::Opto;
    IterPeeps::iterate(stop_);
    // TODO:
    // Optimistic
    return this;
}

CodeGen *CodeGen::typeCheck() {
    // Demand phase Opto for cleaning up dead control flow at least,
    // required for the following GCM.  Note that peeps can be disabled,
    // but still the dead CFG will get cleaned.
    phase_ = Phase::TypeCheck;

    // Type check
    ParserException err = stop_->walk(Node::err());
    if(err != nullptr) {
        throw err;
    }
    return this;
}

CodeGen* CodeGen::GCM() {
    return GCM(false);
}

CodeGen* CodeGen::GCM(bool show) {
    phase_ = Phase::Schedule;

    // Global Code Motion

    // Build the loop tree, fix never-exit loops

    start->buildLoopTree(stop_);
    if(show) {

    }

    // TODO:
    // loop unroll, peel, RCE, etc

    GlobalCodeMotion::buildCFG(start_, stop_);
    return this;
}
CodeGen* CodeGen::localSched() {
    phase_ = Phase::LocalSched;
    ListScheduler::sched(this);
    return this;
}
FunNode* CodeGen::link(TypeFunPtr* tfp) {
    if(auto fun = linker_.get(tfp->make_from(Type::BOTTOM()))) {
        return *fun;
    }
    return nullptr;
}
void CodeGen::link(FunNode* fun) {
    linker_.put(fun->sig()->make_from(Type::BOTTOM()), fun);
}

Node *CodeGen::ctrl() {
    return stop_->ret()->ctrl();
}
Node* CodeGen::expr() {
    return stop_->ret()->expr();
}
std::string CodeGen::print() {
    std::ostringstream b;
    return stop_->print(b).str();
}

std::string CodeGen::ToString() {
    return stop_->p(9999);
}

Node *CodeGen::f(int idx) {
    return stop_->find(idx);
}
