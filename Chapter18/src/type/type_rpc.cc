#include "../../Include/type/type_rpc.h"

TypeRPC::TypeRPC(bool any, Tomi::HashSet<int> rpcs) : Type(TRPC), any_(any), rpcs_(rpcs) {}

}

TypeRPC *TypeRPC::make(
        bool any, Tomi::HashSet<int> rpcs) {

    return alloc.new_object<TypeRPC>(any, rpcs)->intern();
}

TypeRPC *TypeRPC::constant(int cend) {
    Tomi::HashSet<int> rpcs;
    rpcs.insert(cend);
    return make(false, rpcs);
}

TypeRPC *TypeRPC::BOT() {
    return make(true, Tomi::HashSet<int>());
}

TypeRPC *TypeRPC::TEST2() {
    return constant(2);
}

TypeRPC *TypeRPC::TEST3() {
    return constant(3);
}

void TypeRPC::gather(Tomi::Vector<Type *> &ts) {
    ts.push_back(BOT());
    ts.push_back(TEST2());
    ts.push_back(TEST3());
}


std::string TypeRPC::str() {
    if (rpcs_.isEmpty()) return any_ ? "$[ALL]" : "$[]";
    if (rpcs_.size() == 1) {
        for (int rpc: rpcs_) {
            return any_ ? "$[-" + std::to_string(rpc) + "]" : "$[" + std::to_string(rpc) + "]";
        }
    }
    return "$[" + (_any ? "-" : "") + _rpcs + "]";
}

bool TypeRPC::isConstant() {
    return !any_ && rpcs_.size() == 1;
}

Type *TypeRPC::xmeet(Type *that) {
    TypeRPC *rpc = (TypeRPC *) that;
    // If the two sets are equal, the _any must be unequal (invariant),
    // so they cancel and all bits are set.
    if (rpcs_ == rpc->rpcs_) return BOT();
    // Classic union of bit sets (which might be infinite).
    Tomi::HashSet<int> lhs = rpcs_;
    Tomi::HashSet<int> rhs = rpc->rpcs_;

    rhs = rpc->rpcs_;
    // Smaller on left
    if (lhs.size() > rhs.size()) {
        lhs = rpc->rpcs_;
        rhs = rpcs_;
    }

    Tomi::HashSet<int> rpcs;
    bool any = true;
    if (any_ && rpc->any_) {
        for (int i: lhs) {
            if (rhs.constains(i)) rpcs.put(i);
        }
    } else if (!any_ && !rpc->any_) {
        // if neither set is infinite, union.
        any = false;
    } else {
        // If one is infinite, subtract the other from it.
        Tomi::HashSet<int> sub = any_ ? rpc->rpcs_ : rpcs_;
        Tomi::HashSet<int> inf = any_ ? rpcs_ : rpc->rpcs_;
        for (int i: inf) {
            if (inf.get(i) && !sub.contains(i)) rpcs.put(i);
        }
    }
    return make(any, rpcs);
}

Type *TypeRPC::dual() {
    return make(!any_, rpcs_);
}

int TypeRPC::hash() {
    return rpcs_.hashCode() ^ (any_ ? 1 : 0);
}

bool TypeRPC::eq(Type *t) {
    TypeRPC *rpc = (TypeRPC *) t; // Contract
    return any_ == rpc->any_ && rpcs_ == rpc->rpcs_;


}

