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

TEST(SimpleTest, testMulti0
) {
std::string source = R"(
int x, y;
return x+y;
)";
auto *parser = new Parser(source);
StopNode *ret = parser->parse(true)->iterate();
std::ostringstream builder;
std::string result = ret->print(builder).str();
EXPECT_EQ(result,
"return 0;");
}

TEST(SimpleTest, testMulti1
) {
std::string source = R"(
int x=2, y=x+1;
return x+y;
)";
auto *parser = new Parser(source);
StopNode *ret = parser->parse(true)->iterate();
std::ostringstream builder;
std::string result = ret->print(builder).str();
EXPECT_EQ(result,
"return 5;");
}


TEST(SimpleTest, testFinal0
) {
std::string source = R"(
int !x=2;
x=3;
return x;
)";
auto *parser = new Parser(source);
try {
    StopNode *ret = parser->parse(true)->iterate();
} catch (std::runtime_error &e) {
    std::string result = e.what();
    EXPECT_EQ("Cannot reassign final 'x'", result);
}
}

// Todo: Region id should be 21
TEST(SimpleTest, testFinal1
) {
std::string source = R"(
int !x=2, y=3;
if( arg ) { int x = y; x = x*x; y=x; } // Shadow final x
return y;
)";
auto *parser = new Parser(source);
StopNode *ret = parser->parse(true)->iterate();
std::ostringstream builder;
std::string result = ret->print(builder).str();
EXPECT_EQ(result,
"return Phi(Region17,9,3);");
}

TEST(SimpleTest, testConstruct0
) {
std::string source = R"(
struct X { int x=3; };
X z = new X;
return z.x;
)";
auto *parser = new Parser(source);
StopNode *ret = parser->parse(true)->iterate();
std::ostringstream builder;
std::string result = ret->print(builder).str();
EXPECT_EQ(result,
"return 3;");
}


TEST(SimpleTest, testConstruct1
) {
std::string source = R"(
struct X { int !x; };
X z = new X { x=3; };
return z.x;
)";
auto *parser = new Parser(source);
StopNode *ret = parser->parse(true)->iterate();
std::ostringstream builder;
std::string result = ret->print(builder).str();
EXPECT_EQ(result,
"return 3;");
}

TEST(SimpleTest, testConstruct2
) {
std::string source = R"(
struct X { int x=3; };
X z = new X { x = 4; };
return z.x;
)";
auto *parser = new Parser(source);
StopNode *ret = parser->parse(true)->iterate();
std::ostringstream builder;
std::string result = ret->print(builder).str();
EXPECT_EQ(result,
"return 4;");
}

TEST(SimpleTest, testStructFinal
) {
std::string source = R"(
struct Point { int !x, !y; };
Point p = new Point { x=3; y=4; };
return p;
)";
auto *parser = new Parser(source);
StopNode *ret = parser->parse(true)->iterate();
std::ostringstream builder;
std::string result = ret->print(builder).str();
EXPECT_EQ(result,
"return Point;");
}

// Same as the Chapter13 test with the same name, but using the new
// constructor syntax
TEST(SimpleTest, testLinkedList1
) {
std::string source = R"(
struct LLI { LLI? next; int i; };
LLI? head = null;
while( arg ) {
    head = new LLI { next=head; i=arg; };
    arg = arg-1;
}
if( !head ) return 0;
LLI? next = head.next;
if( !next ) return 1;
return next.i;
)";
auto *parser = new Parser(source);
StopNode *ret = parser->parse(true)->iterate();
std::ostringstream builder;
std::string result = ret->print(builder).str();
EXPECT_EQ(result,
"Stop[ return 0; return 1; return .i; ]");
}

TEST(SimpleTest, testLinkedList2
) {
std::string source = R"(
struct LLI { LLI? next; int i; };
LLI? head = null;
while( arg ) {
    head = new LLI {
        next=head;
        // Any old code in the constructor
        int tmp=arg;
        while( arg > 10 ) {
            tmp = tmp + arg;
            arg = arg - 1;
        }
        i=tmp;
    };
    arg = arg-1;
}
if( !head ) return 0;
LLI? next = head.next;
if( !next ) return 1;
return next.i;
)";
auto *parser = new Parser(source);
StopNode *ret = parser->parse(true)->iterate();
std::ostringstream builder;
std::string result = ret->print(builder).str();
EXPECT_EQ(result,
"Stop[ return 0; return 1; return .i; ]");
}

TEST(SimpleTest, testSquare
) {
std::string source = R"(
struct Square {
    flt !side = arg;
    // Newtons approximation to the square root, computed in a constructor.
    // The actual allocation will copy in this result as the initial
    // value for 'diag'.
    flt diag = arg*arg/2;
    while( 1 ) {
        flt next = (side/diag + diag)/2;
        if( next == diag ) break;
        diag = next;
    }
};
return new Square;
)";
auto *parser = new Parser(source);
StopNode *ret = parser->parse(true)->iterate();
std::ostringstream builder;
std::string result = ret->print(builder).str();
EXPECT_EQ(result,
"return Square;");
}
