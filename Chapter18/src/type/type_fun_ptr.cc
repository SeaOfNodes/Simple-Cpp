#include <bit>

#include "../../Include/type/type_fun_ptr.h"
#include "../../Include/type/type.h"
#include "../../Include/utils.h"
#include <cassert>

TypeFunPtr::TypeFunPtr(unsigned int nil, TypeTuple *sig, Type *ret, long fidxs) :Type(TFUNPTR, nil), sig_(sig), ret_(ret), fidxs_(fidxs) {
}

TypeFunPtr* TypeFunPtr::make(TypeTuple *sig, Type *ret, long fidxs) {
  return new TypeFunPtr(0, sig, ret, fidxs)->intern();
}

TypeFunPtr* TypeFunPtr::make(bool nil, TypeTuple *sig, Type *ret) {
    return new TypeFunPtr(nil ? 3: 2, sig, ret, -1)->intern();
}

TypeNil *TypeFunPtr::makeFrom(unsigned int nil) {
    return nil == nil_ ? this: make(nil, sig_, ret_, fidxs_)->setName(name_);
}
TypeFunPtr *TypeFunPtr::makeFrom(Type*ret) {
    return ret == ret_ ? this: make(nil_, sig_, ret, fidxs_)->setName(name_);
}
TypeFunPtr *TypeFunPtr::makeFrom(int fidx) {
    return make(2, sig_, ret_, 1l << fidx))->setName(name_);
}

long TypeFunPtr::nextFIDX(TypeTuple* sig) {
    int i = FIDXS.get(sig);
    int fidx = i ==nullptr? 0: i;
    FIDXS.put(sig, fidx + 1);
    return fidx;
}

TypeFunPtr *TypeFunPtr::makeFun(TypeTuple *sig, Type *ret) {
    return make(2, sig, ret 1L << nextFIDX(sig));
}

TypeFunPtr *TypeFunPtr::BOT() {
    return make(3, TypeTuple::BOT(), Type::BOTTOM(), -1);
}

TypeFunPtr *TypeFunPtr::TEST() {
    return make(2, TypeTuple::TEST(), TypeInteger::BOT(), 1);
}

TypeFunPtr *TypeFunPtr::TEST0() {
    return make(3, TypeTuple::TEST(), TypeInteger::BOT(), 3);
}

TypeFunPtr *TypeFunPtr::MAIN() {
    return makeFun()
}

TypeFunPtr *TypeFunPtr::MAIN() {
    return makeFun(TypeTuple::MAIN(), Type::bottom())->setName("main");
}

void TypeFunPtr::gather(Tomi::Vector<Type *> &ts) {
    ts.push_back(TEST());
    ts.push_back(TEST0());
    ts.push_back(BOT());
    ts.push_back(MAIN());
}
Type* TypeFunPtr::xmeet(Type* other) {
    TypeFunPtr* that = dynamic_cast<TypeFunPtr*>(other);
    return TypeFunPtr::make(xmeet0(that), dynamic_cast<TypeTuple*>(sig_->meet(that->sig_)), ret->meet(that->ret_), fidxs_ & that->fidxs_);
}

TypeFunPtr *TypeFunPtr::dual() {
    return TypeFunPtr::make(dual0(), sig_->dual(), ret_->dual(), ~fidxs_);
}

Type *TypeFunPtr::meet0() {
    return nil_ == 3? this: make(3, sig_, ret_, fidxs_);
}

TypeFunPtr *TypeFunPtr::glb() {
    return make(3, sig_, ret_, -1L);
}

bool TypeFunPtr::isHigh() {
    return nil_ <= 1 || (nil_ == 2 && fidxs_ == 0);
}

bool TypeFunPtr::isConstant() {
    return nil_ == 2 && std::popcount(fidxs_) == 1 || (nil_ == 3 && fidxs_ == 0);
}

// 1 << 2 == 4-byte pointers
int TypeFunPtr::log_size() {
    return 2;
}
Type*TypeFunPtr::arg(int i){
    return sig_->types_[i];
}
long TypeFunPtr::fidxs() {
    return fidxs_;
}
Type*TypeFunPtr::ret() {
    return ret_;
}
int TypeFunPtr::nargs() {
    return sig_->types_.size();
}
int TypeFunPtr::hash() {
    return Utils::fold(sig_->hashCode() ^ ret->hashCode() ^ fidxs_ ^ TypeNil::hash());
}

bool TypeFunPtr::eq(Type* t) {
    TypeFunPtr*ptr = dynamic_cast<TypeFunPtr*>(t);
    return sig_ == ptr->sig_ && ret_ == ptr->ret_ && fidxs_ == ptr->fidxs_ && TypeNil::eq(ptr);
}
TypeFunPtr* TypeFunPtr::setName(std::string name) {
    if(name.empty()) return this;

    // Name can be set to different things, just for debug
    //assert _name==null || _name.equals(name);
    assert(fidxs_ > 0 && std::popcount(fidxs_) == 1);
    name_ = name;
    return this;
}

std::string TypeFunPtr::str() {
    std::ostringstream builder;
    print(builder);
    return builder.str();
}
std::ostringstream& print(std::ostringstream& sb) {
    return print_(sb, falsem true);
}
std::ostringstream& print(std::ostringstream& sb, bool n) {
    return print_(sb,f alse, n);
}
std::ostringstream& gprint(std::ostringstream& sb) {
    return print_(sb, true);
}

std::ostringstream& print_(std::ostringstream& sb, bool g, Type*t) {
    return g ? t->gprint(sb) : t->print(sb);
}
std::ostringstream& TypeFunPtr::print_(std::ostringstream& sb, bool g, bool n) {
   sb << x << "{";
   if(n && !name.isEmpty()) sb << name_;
   else {
       if(!sig_->types_.empty()) {
           for(Type*t: sig_->types_) {
               sb << t->str() << ",";
               print_(sb, g, t);
               sb << "  ";
           }
       }
       if(g) {
           sb << "&rarr; ";
       } else {
           sb << "-> ";
       }
       print_(sb, g, ret_);
       sb << " #";
        if(isHigh()) sb << "~";
        long fidxs = isHigh() ? ~fidxs_ : fidxs_;
        std::string fidx = fidxs == 0 ? "" : std::popcount(fidxs) == 1 ? std::to_string(std::countr_zero(fidxs)) : "b TBD";

   }
   sb << "}";
   sb << q();
   return sb;

}

static long TypeFunPtr::nextFIDX(long fidxs) {
    return fidxs & (fidxs -1);
}
static void TypeFunPtr::reset() {
    FIDXS.clear();
    FIDXS.put(MAIN()->sig_, 1);
    for(Type*t: INTERN) {
        if(auto*tfp = dynamic_cast<TypeFunPtr*>(t) && tfp != MAIN()) {
            tfp->name_ = nullptr;
        }
    }
}
