#include "../Include/graph_evaluator.h"
#include "../Include/graph_visualizer.h"
#include "../Include/IR_printer.h"
#include <gtest/gtest.h>

#include <iostream>
#include <sstream>



TEST(SimpleTest, testJIG
) {
    std::string source = R"(
int i = 0;
i=i=1;
return i;
                                   //return 3.14;

)";
    auto *parser = new Parser(source);
    StopNode *ret = parser->parse(true)->iterate();
    std::ostringstream builder;
    std::string result = ret->print(builder).str();
    EXPECT_EQ(result,
              "return 1;");
}


TEST(SimpleTest, testInc0
) {
std::string source = R"(
return arg++;

)";
auto *parser = new Parser(source);
StopNode *ret = parser->parse(true)->iterate();
std::ostringstream builder;
std::string result = ret->print(builder).str();
EXPECT_EQ(result,
"return arg;");
}


TEST(SimpleTest, testInc1
) {
std::string source = R"(
return arg+++arg++;

)";
auto *parser = new Parser(source);
StopNode *ret = parser->parse(true)->iterate();
std::ostringstream builder;
std::string result = ret->print(builder).str();
EXPECT_EQ(result,
"return ((arg*2)+1);");
}


TEST(SimpleTest, testInc2
) {
std::string source = R"(
//   -(arg--)-(arg--)
return -arg---arg--;

)";
auto *parser = new Parser(source);
StopNode *ret = parser->parse(true)->iterate();
std::ostringstream builder;
std::string result = ret->print(builder).str();
EXPECT_EQ(result,
"return (-((arg*2)+-1));");
}


TEST(SimpleTest, testInc3
) {
std::string source = R"(
int[] !xs = new int[arg];
xs[0]++;
xs[1]++;
return xs[0];

)";
auto *parser = new Parser(source);
StopNode *ret = parser->parse(true)->iterate();
std::ostringstream builder;
std::string result = ret->print(builder).str();
EXPECT_EQ(result,
"return 1;"
);
}


TEST(SimpleTest, testInc4
) {
std::string source = R"(
u8[] !xs = new u8[1];
xs[0]--;
return xs[0];
)";
auto *parser = new Parser(source);
StopNode *ret = parser->parse(true)->iterate();
std::ostringstream builder;
std::string result = ret->print(builder).str();
EXPECT_EQ(result,
"return 255;"
);
}

TEST(SimpleTest, testInc5
) {
std::string source = R"(
struct S { u16 x; };
S !s = new S;
s.x--;
return s.x;
)";
auto *parser = new Parser(source);
StopNode *ret = parser->parse(true)->iterate();
std::ostringstream builder;
std::string result = ret->print(builder).str();
EXPECT_EQ(result,
"return 65535;"
);
}

TEST(SimpleTest, testInc6
) {
std::string source = R"(
return --arg;
)";
auto *parser = new Parser(source);
StopNode *ret = parser->parse(true)->iterate();
std::ostringstream builder;
std::string result = ret->print(builder).str();
EXPECT_EQ(result,
"return (arg+-1);"
);
}

TEST(SimpleTest, testInc7
) {
std::string source = R"(
u8 x=0; return --x;
)";
auto *parser = new Parser(source);
StopNode *ret = parser->parse(true)->iterate();
std::ostringstream builder;
std::string result = ret->print(builder).str();
EXPECT_EQ(result,
"return 255;"
);
}

TEST(SimpleTest, testInc8
) {
std::string source = R"(
int x; x+=2; return x+=3;
)";
auto *parser = new Parser(source);
StopNode *ret = parser->parse(true)->iterate();
std::ostringstream builder;
std::string result = ret->print(builder).str();
EXPECT_EQ(result,
"return 5;");
}

// ---------------------------------------------------------------

TEST(SimpleTest, testVar0
) {
std::string source = R"(
var d; return d;
)";
auto *parser = new Parser(source);
try {
StopNode *ret = parser->parse(true)->iterate();
} catch(std::runtime_error &e) {
    std::string r = e.what();
    EXPECT_EQ(r, "Syntax error, expected =expression:");
}}


TEST(SimpleTest, testVar1
) {
std::string source = R"(
val d; return d;
)";
auto *parser = new Parser(source);
try {
StopNode *ret = parser->parse(true)->iterate();
} catch(std::runtime_error &e) {
std::string r = e.what();
EXPECT_EQ(r, "Syntax error, expected =expression: ;");
}}


TEST(SimpleTest, testVar2
) {
std::string source = R"(
int x; x=3; x++; return x; // Ok, no initializer so x is mutable
)";
auto *parser = new Parser(source);
StopNode *ret = parser->parse(true)->iterate();
std::ostringstream builder;
std::string result = ret->print(builder).str();
EXPECT_EQ(result,
"return 4;");
}

TEST(SimpleTest, testVar3
) {
std::string source = R"(
int x=3; x++; return x; // Ok, primitive so x is mutable despite initializer
)";
auto *parser = new Parser(source);
StopNode *ret = parser->parse(true)->iterate();
std::ostringstream builder;
std::string result = ret->print(builder).str();
EXPECT_EQ(result,
"return 4;");
}

TEST(SimpleTest, testVar4
) {
std::string source = R"(
struct S{int x;}; S s; s=new S; s.x++; return s.x; // Ok, no initializer so x is mutable
)";
auto *parser = new Parser(source);
StopNode *ret = parser->parse(true)->iterate();
std::ostringstream builder;
std::string result = ret->print(builder).str();
EXPECT_EQ(result,
"return 1;");
}

TEST(SimpleTest, testVar5
) {
std::string source = R"(
struct S{int x;}; S s; s=new S{x=3;}; s.x++; return s.x; // Ok, no initializer so x is mutable
)";
auto *parser = new Parser(source);
StopNode *ret = parser->parse(true)->iterate();
std::ostringstream builder;
std::string result = ret->print(builder).str();
EXPECT_EQ(result,
"return 4;");
}

TEST(SimpleTest, testVar6
) {
std::string source = R"(
struct S{int x;}; S s=new S; s.x++; return s.x; // Error initializer so x is immutable
)";
auto *parser = new Parser(source);
try {
StopNode *ret = parser->parse(true)->iterate();
} catch(std::runtime_error &e) {
    std::string r = e.what();
    EXPECT_EQ(r, "Cannot modify final field 'x'");
}

}

TEST(SimpleTest, testVar7
) {
std::string source = R"(
struct S{int x;}; S s=new S{x=3;}; s.x++; return s.x; // Error initializer so x is immutable
)";
auto *parser = new Parser(source);
try {
StopNode *ret = parser->parse(true)->iterate();
} catch(std::runtime_error &e) {
std::string r = e.what();
EXPECT_EQ(r, "Cannot modify final field 'x'");
}

}

TEST(SimpleTest, testVar8
) {
std::string source = R"(
struct S{int x;}; S !s=new S; s.x++; return s.x; // Ok, has '!' so s.x is mutable
)";
auto *parser = new Parser(source);
StopNode *ret = parser->parse(true)->iterate();
std::ostringstream builder;
std::string result = ret->print(builder).str();
EXPECT_EQ(result, "return 1;");
}


TEST(SimpleTest, testVar9
) {
std::string source = R"(
struct S{int x;}; var s=new S; s.x++; return s.x; // Ok, has var so s.x is mutable
)";
auto *parser = new Parser(source);
StopNode *ret = parser->parse(true)->iterate();
std::ostringstream builder;
std::string result = ret->print(builder).str();
EXPECT_EQ(result, "return 1;");
}


TEST(SimpleTest, testVar10
) {
std::string source = R"(
struct S{int x;}; val s=new S; s.x++; return s.x; // Error, has val so x is immutable
)";
auto *parser = new Parser(source);
try {
StopNode *ret = parser->parse(true)->iterate();
} catch(std::runtime_error &e) {
std::string r = e.what();
EXPECT_EQ(r, "Cannot modify final field 'x'");
}

}

TEST(SimpleTest, testVar11
) {
std::string source = R"(
struct Bar { int x; };
Bar !bar = new Bar;
bar.x = 3; // Ok, bar is mutable

struct Foo { Bar? !bar; int y; };
Foo !foo = new Foo { bar = bar; };
foo.bar = bar; // Ok foo is mutable
foo.bar.x++;   // Ok foo and foo.bar and foo.bar.x are all mutable

val xfoo = foo; // Throw away mutability
xfoo.bar.x++;   // Error, cannot mutate through xfoo
)";
auto *parser = new Parser(source);
try {
StopNode *ret = parser->parse(true)->iterate();
} catch(std::runtime_error &e) {
std::string r = e.what();
EXPECT_EQ(r, "Cannot modify final field 'x'");
}

}

TEST(SimpleTest, testVar12
) {
std::string source = R"(
struct Bar { int x; };
Bar !bar = new Bar;
bar.x = 3; // Ok, bar is mutable

struct Foo { Bar? !bar; int y; };
Foo !foo = new Foo;
foo.bar = bar; // Ok bar is mutable
foo.bar.x++;   // Ok foo and foo.bar and foo.bar.x are all mutable

val xfoo = foo;        // Throw away mutability
int x4 = xfoo.bar.x;   // Ok to read through xfoo, gets 4
foo.bar.x++;           // Bumps to 5
int x5 = xfoo.bar.x;   // Ok to read through xfoo, gets 5
return x4*10+x5;
)";
auto *parser = new Parser(source);
StopNode *ret = parser->parse(true)->iterate();
std::ostringstream builder;
std::string result = ret->print(builder).str();
EXPECT_EQ(result, "return 45;");
}


TEST(SimpleTest, testVar13
) {
std::string source = R"(
int i,i++;
)";
auto *parser = new Parser(source);
try {
StopNode *ret = parser->parse(true)->iterate();
} catch(std::runtime_error &e) {
std::string r = e.what();
EXPECT_EQ(r, "Redefining name 'i'");
}
}

TEST(SimpleTest, testVar14
) {
std::string source = R"(
struct B {};
struct A { B b; };
A x = new A {
    return b; // read before init
    b = new B;
};
)";
auto *parser = new Parser(source);
try {
StopNode *ret = parser->parse(true)->iterate();
} catch(std::runtime_error &e) {
std::string r = e.what();
EXPECT_EQ(r, "Cannot read uninitialized field 'b'");
}
}

TEST(SimpleTest, testVar15
) {
std::string source = R"(
struct B {};
struct A { B b; };
return new A {
    if (arg) b = new B; // Constructor ends with partial init of b
}.b;
)";
auto *parser = new Parser(source);
try {
StopNode *ret = parser->parse(true)->iterate();
} catch(std::runtime_error &e) {
std::string r = e.what();
EXPECT_EQ(r, "A is not fully initialized, field 'b' needs to be set in a constructor");
}
}

// ---------------------------------------------------------------


TEST(SimpleTest, testTrinary0
) {
std::string source = R"(
return arg ? 1 : 2;
)";
auto *parser = new Parser(source);
StopNode *ret = parser->parse(true)->iterate();
std::ostringstream builder;
std::string result = ret->print(builder).str();
EXPECT_EQ(result, "return Phi(Region18,1,2);");
}


TEST(SimpleTest, testTrinary1
) {
std::string source = R"(
return arg ? 0 : arg;
)";
auto *parser = new Parser(source);
StopNode *ret = parser->parse(true)->iterate();
std::ostringstream builder;
std::string result = ret->print(builder).str();
EXPECT_EQ(result, "return 0;");
}


TEST(SimpleTest, testTrinary2
) {
std::string source = R"(
struct Bar { int x; };
var b = arg ? new Bar : null;
return b ? b.x++ + b.x++ : -1;
)";
auto *parser = new Parser(source);
StopNode *ret = parser->parse(true)->iterate();
std::ostringstream builder;
std::string result = ret->print(builder).str();
EXPECT_EQ(result, "return Phi(Region53,1,-1);");
}


TEST(SimpleTest, testTrinary3
) {
std::string source = R"(
struct Bar { int x; };
var b = arg ? new Bar;
return b ? b.x++ + b.x++ : -1;
)";
auto *parser = new Parser(source);
StopNode *ret = parser->parse(true)->iterate();
std::ostringstream builder;
std::string result = ret->print(builder).str();
EXPECT_EQ(result, "return Phi(Region53,1,-1);");
}

TEST(SimpleTest, testTrinary4
) {
std::string source = R"(
struct Bar { Bar? next; int x; };
var b = arg ? new Bar { next = (arg==2) ? new Bar{x=2;}; x=1; };
return b ? b.next ? b.next.x : b.x;
)";
auto *parser = new Parser(source);
StopNode *ret = parser->parse(true)->iterate();
std::ostringstream builder;
std::string result = ret->print(builder).str();
EXPECT_EQ(result, "return Phi(Region,Phi(Region,.x,.x),0);");
}


TEST(SimpleTest, testTrinary5
) {
std::string source = R"(
flt f=arg?1:1.2;
return f;   // missing widening
)";
auto *parser = new Parser(source);
StopNode *ret = parser->parse(true)->iterate();
std::ostringstream builder;
std::string result = ret->print(builder).str();
EXPECT_EQ(result, "return Phi(Region20,1.0,1.2);");
}


// ---------------------------------------------------------------


TEST(SimpleTest, testFor0
) {
std::string source = R"(
int sum=0;
for( int i=0; i<arg; i++ )
    sum += i;
return sum;
)";
auto *parser = new Parser(source);
StopNode *ret = parser->parse(true)->iterate();
std::ostringstream builder;
std::string result = ret->print(builder).str();
EXPECT_EQ(result, "return Phi(Loop10,0,(Phi_sum+Phi(Loop,0,(Phi_i+1))));");
}


TEST(SimpleTest, testFor1
) {
std::string source = R"(
int sum=0, i=0;
for( ; i<arg; i++ )
    sum += i;
return sum;
)";
auto *parser = new Parser(source);
StopNode *ret = parser->parse(true)->iterate();
std::ostringstream builder;
std::string result = ret->print(builder).str();
EXPECT_EQ(result, "return Phi(Loop10,0,(Phi_sum+Phi(Loop,0,(Phi_i+1))));");
}


TEST(SimpleTest, testFor2
) {
std::string source = R"(
int sum=0;
for( int i=0; ; i++ ) {
    if( i>=arg ) break;
    sum += i;
}
return sum;
)";
auto *parser = new Parser(source);
StopNode *ret = parser->parse(true)->iterate();
std::ostringstream builder;
std::string result = ret->print(builder).str();
EXPECT_EQ(result, "return Phi(Loop10,0,(Phi_sum+Phi(Loop,0,(Phi_i+1))));");
}

TEST(SimpleTest, testFor3
) {
std::string source = R"(
int sum=0;
for( int i=0; i<arg; )
    sum += i++;
return sum;
)";
auto *parser = new Parser(source);
StopNode *ret = parser->parse(true)->iterate();
std::ostringstream builder;
std::string result = ret->print(builder).str();
EXPECT_EQ(result, "return Phi(Loop10,0,(Phi_sum+Phi(Loop,0,(Phi_i+1))));");
}


TEST(SimpleTest, testFor4
) {
std::string source = R"(
int sum=0;
for( int i=0; i<arg; i++ )
    sum += i;
return i;
)";
auto *parser = new Parser(source);
try {
StopNode *ret = parser->parse(true)->iterate();
} catch(std::runtime_error &e) {
std::string r = e.what();
EXPECT_EQ(r, "Undefined name 'i'");
}
}


TEST(SimpleTest, testFor5
) {
std::string source = R"(
for(;;arg++;) {}
)";
auto *parser = new Parser(source);
try {
StopNode *ret = parser->parse(true)->iterate();
} catch(std::runtime_error &e) {
std::string r = e.what();
EXPECT_EQ(r, "Syntax error, expected code after expression: ;");
}
}

// ---------------------------------------------------------------

TEST(SimpleTest, testForward0
) {
std::string source = R"(
struct A{
    B? f1;
    B? f2;
};
return new A;
)";
auto *parser = new Parser(source);
StopNode *ret = parser->parse(true)->iterate();
std::ostringstream builder;
std::string result = ret->print(builder).str();
EXPECT_EQ(result, "return A;");
}


// Todo: Error
// Todo:: NO MONOTONIC PROGRESS - needs fix
// expr.type_ and def.type()->hash to be different
// Range based for loop bug
TEST(SimpleTest, testLinkedList2
) {
std::string source = R"(
struct LLI { LLI? next; int i; };
LLI? !head = null;
while( arg-- )
    head = new LLI { next=head; i=arg; };
int sum=0;
var ptr = head; // A read-only ptr, to be assigned from read-only next fields
for( ; ptr; ptr = ptr.next )
    sum += ptr.i;
return sum;
)";
auto *parser = new Parser(source);
StopNode *ret = parser->parse(true)->iterate();
std::ostringstream builder;
std::string result = ret->print(builder).str();
EXPECT_EQ(result,
"Stop[ return 0; return 1; return .i; ]");
}

TEST(SimpleTest, testForward1
) {
std::string source = R"(
struct A{
    B?[]? nil_array_of_b;
    B?[]  not_array_of_b = new B?[0];
};
return new A.not_array_of_b;
)";
auto *parser = new Parser(source);
StopNode *ret = parser->parse(true)->iterate();
std::ostringstream builder;
std::string result = ret->print(builder).str();
EXPECT_EQ(result, "return (const)[*B?];");
}


// Todo: should be [int] without the `_` prefix.
TEST(SimpleTest, sieveOfEratosthenes
) {
std::string source = R"(
var ary = new bool[arg], primes = new int[arg];
var nprimes=0, p=0;
// Find primes while p^2 < arg
for( p=2; p*p < arg; p++ ) {
    // skip marked non-primes
    while( ary[p] ) p++;
    // p is now a prime
    primes[nprimes++] = p;
    // Mark out the rest non-primes
    for( int i = p + p; i < ary#; i += p )
        ary[i] = true;
}
// Now just collect the remaining primes, no more marking
for( ; p < arg; p++ )
    if( !ary[p] )
        primes[nprimes++] = p;
// Copy/shrink the result array
var !rez = new int[nprimes];
for( int j=0; j<nprimes; j++ )
    rez[j] = primes[j];
return rez;
)";
auto *parser = new Parser(source);
StopNode *ret = parser->parse(true)->iterate();
std::ostringstream builder;
std::string result = ret->print(builder).str();
EXPECT_EQ(result, "return [_int];");
}