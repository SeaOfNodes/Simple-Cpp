#ifndef TYPE_STRUCT_H
#define TYPE_STRUCT_H
#include "../type/type.h"
#include "../type/field.h"
#include <cassert>
#include <optional>

// A Struct has a name and a set of fields; the fields themselves have
// names and types.  Briefly during parsing its allowed to have a
// forward-ref to a Struct; in this case the _fields array is null.
// Its illegal to attempt to load a field from a forward-ref struct.
//
// During the normal optimization run, struct types "bottom out" at further
// struct references, so we don't have to handle e.g.  cyclic types.  The
// "bottom out" is again the forward-ref struct.
class TypeStruct: public Type {
public:
    std::string name_;
    std::optional<Tomi::Vector<Field*>> fields_;
    explicit TypeStruct(std::string name, std::optional<Tomi::Vector<Field*>> fields);
    // Forward-ref version
    static TypeStruct* make(std::string name);
    // All fields directly listed
    static TypeStruct* make(std::string name, Tomi::Vector<Field*> fields);
    static TypeStruct* TOP();
    static TypeStruct* BOT();
    static TypeStruct* test();

    static TypeStruct* S1F();
    static TypeStruct* S2F();
    static TypeStruct* S1();
    static TypeStruct* S2();

    static void gather(Tomi::Vector<Type*>& ts);

    int find(std::string fname);
    Type* xmeet(Type* that) override;

    TypeStruct* dual();
    TypeStruct* glb();
    bool glb_();

    bool eq(Type* t) override;

    int hash()  override;
    std::ostringstream &print_1(std::ostringstream &builder) override;
    std::string str() override;
};

#endif