#ifndef TYPE_RPC_H
#define TYPE_RPC_H

#include "../type/type.h"
#include "../../Include/tomi.h"
class TypeRPC : public Type {
public:
    // A set of CallEndNode IDs (or StopNode); commonly just one.
    // Basically a sparse bit set
    Tomi::HashSet<int> rpcs_;
    bool any_;
    TypeRPC(bool any, Tomi::HashSet<int> rpcs);

    static TypeRPC* make(bool any, Tomi::HashSet<int> rpcs) override;
    TypeRPC* constant(int cend);

    TypeRPC*BOT();
    TypeRPC*TEST2();
    TypeRPC*TEST3();

    static void gather(Tomi::Vector<Type *> &ts);

    std::string str() override;
    bool isConstant() override;

    Type* xmeet(Type* that) override;

    Type*dual() override;

    int hash() override;

    bool eq(Type*t) override;

};
#endif