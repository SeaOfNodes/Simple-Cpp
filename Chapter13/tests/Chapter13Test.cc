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


TEST(SimpleTest, testLinkedList0
) {
std::string source = R"(
struct LLI { LLI? next; int i; }
LLI? head = null;
while( arg ) {
    LLI x = new LLI;
    x.next = head;
    x.i = arg;
    head = x;
    arg = arg-1;
}
return head.next.i;
)";
auto *parser = new Parser(source);
try {
StopNode *ret = parser->parse(true)->iterate();
} catch (
std::exception &e
) {
std::string error = e.what();
EXPECT_EQ(error,
"Might be null accessing 'next'");
}
}

TEST(SimpleTest, testLinkedList1
) {
std::string source = R"(
struct LLI { LLI? next; int i; }
LLI? head = null;
while( arg ) {
    LLI x = new LLI;
    x.next = head;
    x.i = arg;
    head = x;
    arg = arg-1;
}
if( !head ) return 0;
LLI? next = head.next;
if( next==null ) return 1;
return next.i;
)";
auto *parser = new Parser(source);
StopNode *ret = parser->parse(true)->iterate();
EXPECT_EQ("Stop[ return 0; return 1; return .i; ]", ret->ToString());
}


TEST(SimpleTest,testCoRecur
) {
std::string source = R"(
struct int0 { int i; flt0? f; }
struct flt0 { flt f; int0? i; }
int0 i0 = new int0;
i0.i = 17;
flt0 f0 = new flt0;
f0.f = 3.14;
i0.f = f0;
f0.i = i0;
return f0.i.f.i.i;
)";
auto *parser = new Parser(source);
StopNode *ret = parser->parse(true)->iterate();
EXPECT_EQ("return 17;", ret->ToString());
}


TEST(SimpleTest,testNullRef0
) {
std::string source = R"(
struct N { N next; int i; }
N n = new N;
return n.next;
)";
auto *parser = new Parser(source);
StopNode *ret = parser->parse(true)->iterate();
EXPECT_EQ("return null;", ret->ToString());
}



TEST(SimpleTest,testNullRef1
) {
std::string source = R"(
struct N { N next; int i; }
N n = new N;
n.next = new N;
return n.next;
)";
auto *parser = new Parser(source);
StopNode *ret = parser->parse(true)->iterate();
EXPECT_EQ("return new N;", ret->ToString());
}

TEST(SimpleTest,testNullRef2
) {
std::string source = R"(
struct N { N next; int i; }
N n = new N;
n.next = null;
return n.next;
)";
auto *parser = new Parser(source);
try {
StopNode *ret = parser->parse(true)->iterate();
} catch (std::exception &e) {
    std::string error = e.what();
    EXPECT_EQ(error, "Cannot store null into field *N next");
}
}


TEST(SimpleTest,testNullRef3
) {
std::string source = R"(
struct N { N next; int i; }
N n = new N;
n.i = 3.14;
return n.i;
)";
auto *parser = new Parser(source);
try {
StopNode *ret = parser->parse(true)->iterate();
} catch (std::exception &e) {
std::string error = e.what();
EXPECT_EQ(error, "Cannot store 3.14 into field IntBot i");
}
}

TEST(SimpleTest,testEmpty
) {
std::string source = R"(
struct S{};
)";
auto *parser = new Parser(source);
StopNode *ret = parser->parse(true)->iterate();
EXPECT_EQ("return 0;", ret->ToString());
}

TEST(SimpleTest,testForwardRef0
) {
std::string source = R"(
struct S1 { S2 s; }
return new S2;
)";
auto *parser = new Parser(source);
try{
StopNode *ret = parser->parse(true)->iterate();
} catch (
std::exception &e
) {
    std::string error = e.what();
EXPECT_EQ("Unknown struct type 'S2", error);
}
}


TEST(SimpleTest,testForwardRef1
) {
std::string source = R"(
struct S1 { S2? s; }
struct S2 { int x; }
return new S1.s=new S2;
)";
auto *parser = new Parser(source);
StopNode *ret = parser->parse(true)->iterate();
EXPECT_EQ("return new S1;", ret->ToString());
}

TEST(SimpleTest,testcheckNull
) {
std::string source = R"(
struct I {int i;}
struct P {I pi;}
P p1 = new P;
P p2 = new P;
p2.pi = new I;
p2.pi.i = 2;
if (arg) p1 = new P;
return p1.pi.i + 1;
)";
auto *parser = new Parser(source);
try {
StopNode *ret = parser->parse(true)->iterate();
} catch (std::exception &e) {
std::string error = e.what();
EXPECT_EQ(error, "Might be null accessing 'i'");
}
}