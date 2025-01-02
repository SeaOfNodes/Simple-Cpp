#include "../Include/graph_evaluator.h"
#include "../Include/graph_visualizer.h"
#include "../Include/IR_printer.h"
#include <gtest/gtest.h>

#include <iostream>
#include <sstream>


TEST(SimpleTest, testJIG
) {
    std::string source = R"(return 3.14;)";
    auto *parser = new Parser(source);
    StopNode *ret = parser->parse(true)->iterate();
    std::ostringstream builder;
    std::string result = ret->print(builder).str();
    EXPECT_EQ(result,
              "return 3.14;");
}

TEST(SimpleTest, testLifeTime) {
    std::string source = R"(
u8[] b = new u8[5];
if (arg) {
    b[0] = b[0] + 1;
} else {
    b[1] = b[1] * 2;
}
return b[0] + b[1];
)";
    auto *parser = new Parser(source);
    StopNode *stop = parser->parse(true)->iterate();
    std::ostringstream builder;
    std::string result = stop->print(builder).str();
    EXPECT_EQ(result, "return Phi(Region69,1,0);");
}


TEST(SimpleTest, testCyclic) {
    std::string source = R"(
struct C {
    C? l;
}
C c = new C;
c.l = c;
return c;
)";
    auto *parser = new Parser(source);
    StopNode *stop = parser->parse(true)->iterate();
    std::ostringstream builder;
    std::string result = stop->print(builder).str();
    EXPECT_EQ(result, "return C;");
}


TEST(SimpleTest, testSafetyCheck) {
    std::string source = R"(
u8[] old = new u8[0];
u8[] output = new u8[1];
int i = 0;
while (i < old#) {
    output[i] = old[i];
    i = i + 1;
}
output[i] = 1;
return output;
)";
    auto *parser = new Parser(source);
    StopNode *stop = parser->parse(true)->iterate();
    std::ostringstream builder;
    std::string result = stop->print(builder).str();
    EXPECT_EQ(result, "return u8[];");
}

TEST(SimpleTest, testBasic1) {
    std::string source = R"(
int[] is = new int[2];
return is[1];
)";
    auto *parser = new Parser(source);
    StopNode *stop = parser->parse(true)->iterate();
    std::ostringstream builder;
    std::string result = stop->print(builder).str();
    EXPECT_EQ(result, "return 0;");
}

TEST(SimpleTest, testBasic2) {
    std::string source = R"(
int[] is = new int[2];
int[] is2 = new int[2];
return is[1];
)";
    auto *parser = new Parser(source);
    StopNode *stop = parser->parse(true)->iterate();
    std::ostringstream builder;
    std::string result = stop->print(builder).str();
    EXPECT_EQ(result, "return 0;");
}

TEST(SimpleTest, testBasic3) {
    std::string source = R"(
int[] a = new int[2];
a[0] = 1;
a[1] = 2;
return a[0];
)";
    auto *parser = new Parser(source);
    StopNode *stop = parser->parse(true)->iterate();
    std::ostringstream builder;
    std::string result = stop->print(builder).str();
    EXPECT_EQ(result, "return 1;");
}

TEST(SimpleTest, testBasic4) {
    std::string source = R"(
struct A { int i; }
A?[] a = new A?[2];
return a;
)";
    auto *parser = new Parser(source);
    StopNode *stop = parser->parse(true)->iterate();
    std::ostringstream builder;
    std::string result = stop->print(builder).str();
    EXPECT_EQ(result, "return *A?[]");
}

TEST(SimpleTest, testBasic5) {
    std::string source = R"(
struct S { int x; flt y; }
// A new S
S s = new S; s.x=99; s.y = 3.14;

// Double-d array of Ss.  Fill in one row.
S?[]?[] iss = new S?[]?[2];
iss[0] = new S?[7];
iss[0][2] = s;

// Now pull out the filled-in value, with null checks
flt rez;
S?[]? is = iss[arg];
if( !is ) rez = 1.2;
else {
    S? i = is[2];
    if( !i ) rez = 2.3;
    else rez = i.y;
}
return rez;
)";
    auto *parser = new Parser(source);
    StopNode *stop = parser->parse(true)->iterate();
    std::ostringstream builder;
    std::string result = stop->print(builder).str();
    EXPECT_EQ(result, "return Phi(Region118,1.2,Phi(Region115,2.3,.y));");
}


TEST(SimpleTest, testBasic6) {
    std::string source = R"(
struct S { int x; flt y; }
// A new S
S s = new S; s.x=99; s.y = 3.14;

// Double-d array of Ss.  Fill in one row.
S?[]?[] iss = new S?[]?[2];
iss[0] = new S?[7];
iss[0][2] = s;

// Now pull out the filled-in value, with null checks
flt rez;
S?[]? is = iss[arg];
if( !is ) rez = 1.2;
else {
    S? i = is[2];
    if( !i ) rez = 2.3;
    else rez = i.y;
}
return rez;
)";
    auto *parser = new Parser(source);
    StopNode *stop = parser->parse(true)->iterate();
    std::ostringstream builder;
    std::string result = stop->print(builder).str();
    EXPECT_EQ(result, "return Phi(Region122,Phi(Region118,.y,1.2),1.2);");
}


TEST(SimpleTest, testTree) {
    std::string source = R"(
// Can we define a forward-reference array?
struct Tree { Tree?[] _kids; }
Tree root = new Tree;
root._kids = new Tree?[2];
root._kids[0] = new Tree;
return root;
)";
    auto *parser = new Parser(source);
    StopNode *stop = parser->parse(true)->iterate();
    std::ostringstream builder;
    std::string result = stop->print(builder).str();
    EXPECT_EQ(result, "return Tree;");
}


TEST(SimpleTest, testNestedStructAddMemProj) {
    std::string source = R"(
struct S {
    int a;
    int[] b;
}
return 0;
)";
    auto *parser = new Parser(source);
    StopNode *stop = parser->parse(true)->iterate();
    std::ostringstream builder;
    std::string result = stop->print(builder).str();
    EXPECT_EQ(result, "return 0;");
}


TEST(SimpleTest, testRollingSum) {
    std::string source = R"(
int[] ary = new int[arg];
// Fill [0,1,2,3,4,...]
int i=0;
while( i < ary# ) {
    ary[i] = i;
    i = i+1;
}
// Fill [0,1,3,6,10,...]
i=0;
while( i < ary# - 1 ) {
    ary[i+1] = ary[i+1] + ary[i];
    i = i+1;
}
return ary[1] * 1000 + ary[3]; // 1 * 1000 + 6
)";
    auto *parser = new Parser(source);
    StopNode *stop = parser->parse(true)->iterate();
    std::ostringstream builder;
    std::string result = stop->print(builder).str();
    EXPECT_EQ(result, "return (.[]+(.[]*1000));");
}


TEST(SimpleTest, sieveOEratosthenes) {
    std::string source = R"(
int[] ary = new int[arg];
int[] primes = new int[arg];
int nprimes = 0;
// Find primes
int j=2;
while( j*j < arg ) {
    while( ary[j]==1 ) j = j + 1;
    // j is now a prime
    primes[nprimes] = j;  nprimes = nprimes + 1;
    // Mark out the rest non-primes
    int i = j + j;
    while( i < ary# ) {
        ary[i] = 1;
        i = i + j;
    }
    j = j + 1;
}
// Now just collect the remaining primes
while( j < arg ) {
    if( ary[j] == 0 ) {
        primes[nprimes] = j;  nprimes = nprimes + 1;
    }
    j = j + 1;
}
// Shrink the result array to size
int[] rez = new int[nprimes];
j = 0;
while( j < nprimes ) {
    rez[j] = primes[j];
    j = j + 1;
}

return rez;
)";
    auto *parser = new Parser(source);
    StopNode *stop = parser->parse(true)->iterate();
    std::ostringstream builder;
    std::string result = stop->print(builder).str();
    EXPECT_EQ(result, "return int[];");
}


TEST(SimpleTest, testNewNodeInit) {
    std::string source = R"(
struct S {int i; flt f;}
S s1 = new S;
S s2 = new S;
s2.i = 3;
s2.f = 2.0;
if (arg) s1 = new S;
return s1.i + s1.f;
)";
    auto *parser = new Parser(source);
    StopNode *stop = parser->parse(true)->iterate();
    std::ostringstream builder;
    std::string result = stop->print(builder).str();
    EXPECT_EQ(result, "return ((flt).i+.f);;");
}


TEST(SimpleTest, testBad0) {
    std::string source = R"(
return new flt;
)";
    auto *parser = new Parser(source);
    try {
        StopNode *stop = parser->parse(true)->iterate();
    } catch (std::runtime_error &e) {
        EXPECT_EQ(e.what(), "Cannot allocate a new flt");
    }
}

TEST(SimpleTest, testBad1) {
    std::string source = R"(
int is = new int[2];
)";
    auto *parser = new Parser(source);
    try {
        StopNode *stop = parser->parse(true)->iterate();
    } catch (std::runtime_error &e) {
        EXPECT_EQ(e.what(), "Type *int[] is not of declared type int");
    }
}

TEST(SimpleTest, testBad2) {
    std::string source = R"(
int[] is = new int[3.14];
return is[1];
)";
    auto *parser = new Parser(source);
    try {
        StopNode *stop = parser->parse(true)->iterate();
    } catch (std::runtime_error &e) {
        EXPECT_EQ(e.what(), "annot allocate an array with length 3.14");
    }
}

TEST(SimpleTest, testBad3) {
    std::string source = R"(
int[] is = new int[arg];
return is[1];
)";
    auto *parser = new Parser(source);
    StopNode *stop = parser->parse(true)->iterate();
    std::ostringstream builder;
    std::string result = stop->print(builder).str();
    EXPECT_EQ(result, "return 0;");
}
