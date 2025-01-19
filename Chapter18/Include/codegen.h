#ifndef CODEGEN_H
#define CODEGEN_H

#include "../Include/node/node.h"
#include "../Include/type/type.h"
#include "../Include/tomi.h"


class TypeInteger;
class Parser;
class StartNode;
class StopNode;

class TypeFunPtr;
class FunNode;

class CodeGen {
public:
    enum class Phase {
        Parse, Opto, TypeCheck, Schedule, LocalSched, RegAlloc
    };
    Phase phase_;
    std::string src_;
    // Compile-time known initial argument type
    TypeInteger* arg_;

    Parser* p;
    StartNode* start_;
    StopNode* stop_;

    // "Linker" mapping from constant TypeFunPtrs to heads of function.  These
    // TFPs all have exact single fidxs and their return is wiped to BOTTOM (so
    // the return is not part of the match).
    Tomi::HashMap<TypeFunPtr*, FunNode*> linker_;

    // Last created CodeGen as a global, for easier debugging prints
    static CodeGen* CODE;

    Codegen(std::string src, TypeInteger* arg);
    Codegen(std::string src);

    CodeGen* parse();
    CodeGen* parse(bool disable);

    CodeGen* opto();
    CodeGen* typeCheck();

    CodeGen* GCM();
    CodeGen* GCM(bool show);

    CodeGen* localSched();

    FunNode* link(TypeFunPtr* tfp);

    Node*ctrl();
    Node*expr();
    std::string print();

    std::string ToString() override;

    Node*f(int idx);


};
#endif