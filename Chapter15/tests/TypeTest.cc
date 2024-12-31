//#include <gtest/gtest.h>
//#include "../Include/type/type.h"
//#include "../Include/type/type_struct.h"
//#include "../Include/type/integer_type.h"
//#include "../Include/type/type_mem_ptr.h"
//#include "../Include/type/type_mem.h"
//
//// forward decl
//static void check_commute(Type* t0, Type* t1);
//static void check_symmetric(Type* t0, Type* t1);
//static void assoc(Type* t0, Type* t1, Type* t2);
//
//TEST(SimpleTest, testTypeAdHoc) {
//    Tomi::Vector<Field*> fields_s1;
//    fields_s1.push_back(new Field("a", TypeInteger::BOT()));
//    fields_s1.push_back(new Field("b", TypeInteger::BOT()));
//    TypeStruct* s1 = TypeStruct::make("s1", fields_s1);
//
//
//    TypeStruct* s2 = TypeStruct::make("s2", fields_s1);
//    // since all fields are bottom
//    // depends on if interning works or not
//    EXPECT_EQ(s1, s1->glb());
//    EXPECT_NE(s1, s1->dual());
//    EXPECT_EQ(s1, s1->dual()->glb());
//    TypeMem* m1 = TypeMem::make(1);
//    TypeMem* m2 = TypeMem::make(2);
//    TypeMem* m3 = TypeMem::make(3);
//    TypeMem* m4 = TypeMem::make(4);
//    // invariant holds they are not equal
//    EXPECT_NE(m1, m2);
//    EXPECT_NE(m2, m3);
//    EXPECT_NE(m3, m4);
//
//    // if the name of the struct are different meet is always bottom
//    EXPECT_EQ(TypeStruct::BOTTOM(), s1->meet(s2));
//    EXPECT_EQ(TypeMem::BOTTOM(), m1->meet(m2));
//    EXPECT_EQ(TypeMem::BOTTOM(), m2->meet(m3));
//    EXPECT_EQ(TypeMem::BOTTOM(), m3->meet(m4));
//
//    EXPECT_EQ(TypeMem::BOT(), m1->glb());
//    EXPECT_EQ(m1, m1->dual());
//    EXPECT_EQ(TypeMem::TOP(), m1->glb()->dual());
//
//    TypeMemPtr* ptr1 = TypeMemPtr::make(s1);
//    EXPECT_EQ(s1, ptr1->obj_);
//    TypeMemPtr* ptr2 = TypeMemPtr::make(s2);
//    EXPECT_EQ(s2, ptr2->obj_);
//
//    TypeMemPtr* ptrnil1 = TypeMemPtr::make(s1, true);
//    EXPECT_EQ(s1, ptrnil1->obj_);
//    EXPECT_EQ(ptrnil1->nil_, true);
//    EXPECT_EQ(ptrnil1->obj_ == nullptr, false);
//
//    TypeMemPtr* ptr2nil = TypeMemPtr::make(s2, true);
//    EXPECT_EQ(s2, ptr2nil->obj_);
//
//    // not equal cause they are not pointing to the same thing
//    EXPECT_NE(ptr1, ptr2);
//    EXPECT_NE(ptr1, ptr1->glb());
//    EXPECT_EQ(ptrnil1, ptr1->glb());
//
//    // Todo: maybe few other stuff here
//    TypeMemPtr* TOP = TypeMemPtr::TOP();
//    TypeMemPtr* BOT = TypeMemPtr::BOT();
//    TypeMemPtr* PTR = TypeMemPtr::VOIDPTR();
//    TypeMemPtr* NULLPTR = TypeMemPtr::NULLPTR();
//
//    Type* PTR_MEET_NULL = NULLPTR->meet(PTR);
//    EXPECT_EQ(BOT, PTR_MEET_NULL);
//    Type* PTR_MEET_PTR = ptr1->meet(ptr2);
//    EXPECT_EQ(PTR, PTR_MEET_PTR);
//    Type* NULL_JOIN_PTR1 = NULLPTR->join(ptr1);
//    EXPECT_EQ(TOP, NULL_JOIN_PTR1);
//    Type* NULL_JOIN_PTR = PTR->join(NULLPTR);
//    EXPECT_EQ(TOP, NULL_JOIN_PTR);
//}
//
//// Test theoretical properties.
//// This is a symmetric complete bounded (ranked) lattice.
//// Also the meet is commutative and associative.
//// The lattice has a dual (symmetric), and join is ~(~x meet ~y).
//// See https://en.wikipedia.org/wiki/Lattice_(order).
//
//TEST(SimpleTest, testLatticeTheory) {
//    Tomi::Vector<Type*> ts;
//    ts = Type::gather();
//    // Confirm commutative & complete
//    for(Type* t: ts) {
//        for(Type* t2: ts) {
//            check_commute(t, t2);
//            check_symmetric(t, t2);
//        }
//    }
//// Confirm associative
//    for(Type*t0: ts) {
//        for (Type* t1: ts) {
//            for (Type* t2: ts) {
//                 assoc(t0, t1, t2);
//            }
//        }
//    }
//// Confirm symmetry.  If A isa B, then A.join(C) isa B.join(C)
//for(Type* t0: ts) {
//    for ( Type* t1: ts) {
//        if(t0->isa(t1)) {
//            for(Type* t2: ts) {
//                Type* t02 = t0->join(t2);
//                Type* t12 = t1->join(t2);
//                Type* mt = t02->meet(t12);
//                EXPECT_EQ(mt, t12);
//            }
//        }
//    }
//}
//}
//
//// By design in meet, args are already flipped to order _type, which forces
//// symmetry for things with badly ordered _type fields.  The question is
//// still interesting for other orders.
//static void check_commute(Type* t0, Type* t1) {
//    if(t0 == t1) return;
//    if(t0->isSimple() && !t1->isSimple()) return; // By design, flipped the only allowed order
//    Type* mta= t0->meet(t1);
//    Type* mtb= t1->meet(t0);  // Reverse args and try again
//    EXPECT_EQ(mta, mtb);
//}
//static void check_symmetric(Type* t0, Type* t1) {
//    if(t1 == t0)  return;
//    Type* mt = t0->meet(t1);
//    Type* ta = mt->dual()->meet(t1->dual());
//    Type* tb = mt->dual()->meet(t0->dual());
//    EXPECT_EQ(ta, t1->dual());
//    EXPECT_EQ(tb, t0->dual());
//}
//
//static void assoc(Type* t0, Type* t1, Type* t2) {
//    Type* t01 = t0->meet(t1);
//    Type* t12 = t1->meet(t2);
//    Type* t01_2 = t01->meet(t2);
//    Type* t0_12 = t0->meet(t12);
//    EXPECT_EQ(t01_2, t0_12);
//}