#include "../../Include/type/tuple_type.h"
#include <iostream>
#include "../../Include/type/integer_type.h"
#include "../../Include/type/type_mem_ptr.h"
#include "../../Include/type/type.h"
#include "../../Include/type/type_mem.h"

TypeTuple::TypeTuple(std::initializer_list<Type *> types) : Type(TTUPLE) {
  for (auto type : types) {
    types_.push_back(type);
  }
}
TypeTuple::TypeTuple(Tomi::Vector<Type *> &types) : Type(TTUPLE) { types_ = types; }

TypeTuple *TypeTuple::START() {
    return make({Type::CONTROL(), TypeMem::TOP(), TypeInteger::BOT()});
}
// O(N)
std::string TypeTuple::ToString() {
  std::ostringstream os;
  os << "Types of the tuple: ";
  for (Type *type : types_) {
    os << type->ToString();
  }
  return os.str();
}
std::string TypeTuple::str() {
    std::ostringstream builder;
    builder << "[  ";
    for (Type *type : types_) {
        builder << type->str() << ", ";
    }
    std::string result = builder.str();
    // Remove the last ", " if the result is not empty and contains extra delimiters
    if (result.size() > 3) { // Minimum size of "[  "
        result.erase(result.size() - 2); // Remove the last ", "
    }

    result += "]";
    return result;
}

std::ostringstream& TypeTuple::typeName(std::ostringstream &builder) {
    builder << "[";
    for (Type *type : types_) {
        type->typeName(builder);
        builder << ",";
    }
    builder << "]";
    return builder;
}
Type* TypeTuple::glb() {
    Tomi::Vector<Type *> ts(types_.size());
    for(int i = 0; i < types_.size(); i++) {
        ts.push_back(types_[i]->glb());
    }
    return make(ts);
}
Type *TypeTuple::xmeet(Type *other) {
  auto *tt = dynamic_cast<TypeTuple *>(other);
  if(types_.size() != tt->types_.size()) {
      return  Type::BOTTOM();
  }
  Tomi::Vector<Type *> ts;
  int cnt = 0;
  for (auto item : types_) {
    ts.push_back(item->meet(tt->types_[cnt]));
    cnt++;
  }
  TypeTuple* smt = make(ts);
  return make(ts);
}


std::ostringstream &TypeTuple::print_1(std::ostringstream &builder) {
  builder << "[";
  // fence-post problem here
  for (auto t : types_) {
    t->print_1(builder);
    builder << ",";
  }
  builder << "]";
  return builder;
}

 int TypeTuple::hash() {
   int sum = 0;
  for (Type *type : types_) {
    sum ^= type->hashCode();
  }
  return sum;
}
TypeTuple *TypeTuple::make(std::initializer_list<Type *> types) {
  return dynamic_cast<TypeTuple *>((alloc.new_object<TypeTuple>(types))->intern());
}
TypeTuple *TypeTuple::make(Tomi::Vector<Type *> types) {
  auto* a = alloc.new_object<TypeTuple>(types);
  a = dynamic_cast<TypeTuple*>(a->intern());
  assert(a); // can't be null must be a valid intern
  return a;

}

bool TypeTuple::eq(Type *t) {
  auto *tt = (TypeTuple *)(t);
  if (types_.size() != tt->types_.size())
    return false;
  for (int i = 0; i < types_.size(); i++) {
    if (types_[i] != tt->types_[i])
      return false;
  }
  return true;
}
TypeTuple* TypeTuple::TEST() {
    return make({TypeInteger::BOT(), TypeMemPtr::test()});
}
void TypeTuple::gather(Tomi::Vector<Type *> &ts) {
    ts.push_back(TEST());
    ts.push_back(START());
}
TypeTuple *TypeTuple::IF_BOTH() {
  static TypeTuple IF_BOTH_INSTANCE({Type::CONTROL(), Type::CONTROL()});
  return (dynamic_cast<TypeTuple*>(IF_BOTH_INSTANCE.intern()));
}

TypeTuple *TypeTuple::IF_NEITHER() {
  static TypeTuple IF_NEITHER_INSTANCE({Type::XCONTROL(), Type::XCONTROL()});
  return (dynamic_cast<TypeTuple*>(IF_NEITHER_INSTANCE.intern()));
}

TypeTuple *TypeTuple::IF_TRUE() {
  static TypeTuple IF_TRUE({Type::CONTROL(), Type::XCONTROL()});
  return (dynamic_cast<TypeTuple*>(IF_TRUE.intern()));
}

TypeTuple *TypeTuple::IF_FALSE() {
  static TypeTuple IF_FALSE({Type::XCONTROL(), Type::CONTROL()});
  return (dynamic_cast<TypeTuple*>(IF_FALSE.intern()));;
}
