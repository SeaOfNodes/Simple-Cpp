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

TEST(SimpleTest, testRange
) {
std::string source = R"(
int b;
if( arg ) b=1; else b=0;
int c = 99;
if( b < 0 ) c = -1;
if( b > 2 ) c =  1;
return c;
)";
auto *parser = new Parser(source);
StopNode *ret = parser->parse(true)->iterate();
std::ostringstream builder;
std::string result = ret->print(builder).str();
EXPECT_EQ(result,
"return 99;");
}

TEST(SimpleTest, testU8
) {
std::string source = R"(
u8 b = 123;
b = b + 456;// Truncate
return b;
)";
auto *parser = new Parser(source);
StopNode *ret = parser->parse(true)->iterate();
std::ostringstream builder;
std::string result = ret->print(builder).str();
EXPECT_EQ(result,
"return 67;");
}

TEST(SimpleTest, testU8While
) {
std::string source = R"(
u8 b = 123;
while( b ) b = b + 456;// Truncate
return b;
)";
auto *parser = new Parser(source);
StopNode *ret = parser->parse(true)->iterate();
std::ostringstream builder;
std::string result = ret->print(builder).str();
EXPECT_EQ(result,
"return Phi(Loop9,123,((Phi_b+456)&255));");
}

TEST(SimpleTest, testU1
) {
std::string source = R"(
bool b = 123;
b = b + 456;// Truncate
u1 c = b;   // No more truncate needed
return c;
)";
auto *parser = new Parser(source);
StopNode *ret = parser->parse(true)->iterate();
std::ostringstream builder;
std::string result = ret->print(builder).str();
EXPECT_EQ(result,
"return 1;");
}

TEST(SimpleTest, testAnd
) {
std::string source = R"(
int b = 123;
b = b+456 & 31;                 // Precedence
return b;
)";
auto *parser = new Parser(source);
StopNode *ret = parser->parse(true)->iterate();
std::ostringstream builder;
std::string result = ret->print(builder).str();
EXPECT_EQ(result,
"return 3;");
}


TEST(SimpleTest, testRefLoad
) {
std::string source = R"(
struct Foo { u1 b; }
Foo f = new Foo;
f.b = 123;
return f.b;
)";
auto *parser = new Parser(source);
StopNode *ret = parser->parse(true)->iterate();
std::ostringstream builder;
std::string result = ret->print(builder).str();
EXPECT_EQ(result,
"return 1;");
}

TEST(SimpleTest, testSigned) {
std::string source = R"(
i8 b = 255;                     // Chopped
return b;                       // Sign extend
)";
auto *parser = new Parser(source);
StopNode *ret = parser->parse(true)->iterate();
std::ostringstream builder;
std::string result = ret->print(builder).str();
EXPECT_EQ(result,
"return -1;");
}

TEST(SimpleTest, testI8 ){
        std::string source = R"(
i8 b = arg;
b = b + 1;// Truncate
return b;
)";
        auto *parser = new Parser(source);
        StopNode *ret = parser->parse(true)->iterate();
        std::ostringstream builder;
        std::string result = ret->print(builder).str();
        EXPECT_EQ(result,
        "return (((((arg<<56)>>56)+1)<<56)>>56);");
}


TEST(SimpleTest, testMask ){
std::string source = R"(
u16 mask = (1<<16)-1;           // AND mask
int c = 123456789 & mask;
return c;                       //
)";
auto *parser = new Parser(source);
StopNode *ret = parser->parse(true)->iterate();
std::ostringstream builder;
std::string result = ret->print(builder).str();
EXPECT_EQ(result,
"return 52501;");
}


TEST(SimpleTest, testWrapMinus ){
std::string source = R"(
int MAX = 9223372036854775807; //0x7FFFFFFFFFFFFFFF;
int i = (arg&MAX) + -MAX + -1; // Basically (e0) + Long.MIN_VALUE
int j = -i; // Negating Long.MIN_VALUE wraps, cannot constant fold
if (arg) j = 1;
return j;
)";
auto *parser = new Parser(source);
StopNode *ret = parser->parse(true)->iterate();
std::ostringstream builder;
std::string result = ret->print(builder).str();
EXPECT_EQ(result,
"return Phi(Region28,1,(-((arg&9223372036854775807)+-9223372036854775808)));");
}

TEST(SimpleTest, testWrapShr ){
std::string source = R"(
return (arg >>> 1)==0;
)";
auto *parser = new Parser(source);
StopNode *ret = parser->parse(true)->iterate();
std::ostringstream builder;
std::string result = ret->print(builder).str();
EXPECT_EQ(result,
"return (!(arg>>>1));");
}

TEST(SimpleTest, testOr ){
std::string source = R"(
return (arg | 123 ^ 456) >>> 1;
)";
auto *parser = new Parser(source);
StopNode *ret = parser->parse(true)->iterate();
std::ostringstream builder;
std::string result = ret->print(builder).str();
EXPECT_EQ(result,
"return (((arg|123)^456)>>>1);");
}

TEST(SimpleTest, testMaskFloat ){
std::string source = R"(
flt f = arg;
arg = f & 0;
return arg;
)";
auto *parser = new Parser(source);
try {
StopNode *ret = parser->parse(true)->iterate();
} catch (std::exception &e) {
    std::string result = e.what();
    EXPECT_EQ(result, "Cannot '&' FltBot");
}
}

TEST(SimpleTest, testLoadBug ){
std::string source = R"(
struct A { int i; }
struct B { int i; }
A a = new A;
A t1 = new A;
B b = new B;
B t2 = new B;
int i;
if (arg) i = a.i;
else     i = b.i;
return i;
)";
auto *parser = new Parser(source);
StopNode *ret = parser->parse(true)->iterate();
std::ostringstream builder;
std::string result = ret->print(builder).str();
EXPECT_EQ(result,
"return Phi(Region31,.i,.i);");
}

TEST(SimpleTest, testBug2 ){
std::string source = R"(
int z = 0;
while (1) {
    int j;
    if (arg&3) {
        j = arg >> 2;
    } else {
        j = (arg >> 3)+z;
    }
    return j+1;
}
)";
auto *parser = new Parser(source);
StopNode *ret = parser->parse(true)->iterate();
std::ostringstream builder;
std::string result = ret->print(builder).str();
EXPECT_EQ(result,
"return ((arg>>Phi(Region32,2,3))+1);");
}

TEST(SimpleTest, testBug3 ){
std::string source = R"(
flt f = arg;
bool b;
if (arg&3) b = f == 1.0;
else       b = f == 2.0;
if (arg&5) b = arg == 1;
return b;
)";
auto *parser = new Parser(source);
StopNode *ret = parser->parse(true)->iterate();
std::ostringstream builder;
std::string result = ret->print(builder).str();
EXPECT_EQ(result,
"return Phi(Region37,(arg==1),((flt)arg==Phi(Region23,1.0,2.0)));");
}

TEST(SimpleTest, testBug4 ){
std::string source = R"(
int i;
if (arg&7) i=3;
else       i=2;
return (arg == i) == 1;
)";
auto *parser = new Parser(source);
StopNode *ret = parser->parse(true)->iterate();
std::ostringstream builder;
std::string result = ret->print(builder).str();
EXPECT_EQ(result,
"return (arg==Phi(Region18,3,2));");
}

TEST(SimpleTest, testBug5 ){
std::string source = R"(
int d = -1;
while (1) {
    int i = (arg & 7) + (9223372036854775807 - 7);
    if (arg & 1) i = ((arg&3) + (9223372036854775807 - 3)) & d;
    return i + -1;
}
)";
auto *parser = new Parser(source);
StopNode *ret = parser->parse(true)->iterate();
std::ostringstream builder;
std::string result = ret->print(builder).str();
EXPECT_EQ(result,
"return (Phi(Region43,((arg&3)+9223372036854775804),((arg&7)+9223372036854775800))+-1);");
}

TEST(SimpleTest, testTypes ){
std::string source = R"(
i8  xi8  = 123456789;  if( xi8  !=        21 ) return -8;
i16 xi16 = 123456789;  if( xi16 !=    -13035 ) return -16;
i32 xi32 = 123456789;  if( xi32 != 123456789 ) return -32;
i64 xi64 = 123456789;  if( xi64 != 123456789 ) return -64;
int xint = 123456789;  if( xint != 123456789 ) return -64;

u1  ui1  = 123456789;  if( ui1  !=         1 ) return 1;
u8  ui8  = 123456789;  if( ui8  !=        21 ) return 8;
u16 ui16 = 123456789;  if( ui16 !=     52501 ) return 16;
u32 ui32 = 123456789;  if( ui32 != 123456789 ) return 32;

flt fflt = 3.141592653589793;  if( fflt != 3.141592653589793 ) return 3;
f64 ff64 = 3.141592653589793;  if( ff64 != 3.141592653589793 ) return 3;
f32 ff32 = 3.141592653589793;  if( ff32 != 3.1415927410125732) return 5;

return 0;
)";
auto *parser = new Parser(source);
StopNode *ret = parser->parse(true)->iterate();
std::ostringstream builder;
std::string result = ret->print(builder).str();
EXPECT_EQ(result,
"return 0;");
}