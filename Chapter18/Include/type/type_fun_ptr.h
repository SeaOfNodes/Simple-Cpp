#ifndef TYPE_FUN_PTR_H
#define TYPE_FUN_PTR_H

#include "type.h"
#include "tuple_type.h"
#include "type_nil.h"
/**
 * Represents a Pointer to a function.
 * <p></p>
 * Functions have argument types and a return type, making a function
 * signature.  Within a signature there can be many instances of functions, and
 * each function is labeled with a small integer constant.  A TFP can represent
 * a single signature and a set of functions; the set might contain only 1
 * function or the zeroth function (the null ptr).
 * <p></p>
 * Functions with different signatures cannot be mixed, and will result in a
 * bottom function type, which can only be null-checked.
 */

class TypeFunPtr: public TypeNil {
public:
    // A TypeFunPtr is Signature and a set of functions.
    // A TypeFunPtr is Signature and a set of functions.
    TypeTuple* sig_;
    Type* ret_;
    // Cheesy easy implementation for a small set; 1 bit per unique function
    // within the TypeTuple.  Can be upgraded to a BitSet for larger classes of
    // functions.  Negative means "these 63 concrete bits plus infinite unknown more"

    long fidxs_;  // 63 unique functions per signature
    std::string name_; // Optional debug function name; only for named single functions
    TypeFunPtr(unsigned int nil, TypeTuple* sig, Type*ret, long fidxs);

    static TypeFunPtr*make(unsigned int nil, TypeTuple* sig, Type*ret, long fidxs);
    static TypeFunPtr*make(bool nil, TypeTuple* sig, Type*ret);
    TypeFunPtr* make_from(unsigned int nil);
    TypeFunPtr* make_from(Type*ret);
    TypeFunPtr*make_from(int fidx);

    // Compute "function indices": FIDX
    Tomi::HashMap<TypeTuple*, int> FIDXS;
    static int nextFIDX(TypeTuple* sig);

    static TypeFunPtr*makeFun(TypeTuple* sig, Type*ret);

    static TypeFunPtr* make_from(TypeTuple* sig, Type*ret);
    static TypeFunPtr* BOT();
    static TypeFunPtr* TEST();
    static TypeFunPtr* TEST0();
    static TypeFunPtr* MAIN();
    static void gather(Tomi::Vector<Type*>& ts);

    Type*xmeet(Type*other) override;
    TypeFunPtr* dual() override;
    Type*meet0(Type*other) override;
    TypeFunPtr* glb() override;

    bool isHigh() override;
    bool isConstant() override;

    int log_size() override;

    Type*arg(int i);
    long fidxs();
    Type*ret();
    int nargs();

    int hash() override;

    bool eq() override;
    TypeFunPtr*setName(std::string name);

    std::string str() override;
    std::ostringstream&print(std::ostringstream&os) override;
    std::ostringstream& print(std::ostringstream& sb, bool n);
    std::ostringstream& gprint(std::ostringstream& sb) override;
    static long nextFIDX(long fidxs);
    static void reset();
private:
    static std::ostringstream& print_(std::ostringstream& sb, bool g, Type*t);
    static std::ostringstream& print_(std::ostringstream& sb, bool g, bool n);

};
#endif