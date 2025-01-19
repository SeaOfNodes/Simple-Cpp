#ifndef VAR_H
#define VAR_H

#include "parser.h"

class Var  {
    std::string name_; // Declared name
    int idx_; // index in containing scope
    Type* type_; // Declared type
    bool final_; // Final field
    Lexer loc_; // Ssource location
    bool fref_; // Forward reference

public: Var(int idx, std::string name, Type*type, bool xfinal, Lexer loc);
     Var(int idx, std::string name, Type*type, bool xfinal, Lexer loc, bool fref);
     Type*type();
     Type* lazyGLB();

    // Forward reference variables (not types) must be BOTTOM and
    // distinct from inferred variables
    bool isFref();

    void defFRef(Type*type, bool xfinal, Lexer loc);
    std::string ToString();

};
#endif