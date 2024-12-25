#include "../Include/graph_evaluator.h"
#include "../Include/graph_visualizer.h"
#include "../Include/IR_printer.h"
#include <gtest/gtest.h>

#include <iostream>
#include <sstream>


TEST(SimpleTest, testJIG
) {
std::string source = R"(
    return 3.14;
)";
auto *parser = new Parser(source);
StopNode *ret = parser->parse(true)->iterate();
std::ostringstream builder;
std::string result = ret->print(builder).str();
EXPECT_EQ(result,
"return 3.14");
}


TEST(SimpleTest, testFloat
) {
std::string source = R"(
    return 3.14;
)";
auto *parser = new Parser(source);
StopNode *ret = parser->parse(true)->iterate();
std::ostringstream builder;
std::string result = ret->print(builder).str();
EXPECT_EQ(result,
"return 3.14");
}


TEST(SimpleTest, testSquareRoot
) {
std::string source = R"(
flt guess = arg;
while( 1 ) {
    flt next = (arg/guess + guess)/2;
    if( next == guess ) break;
    guess = next;
}
return guess;
)";
auto *parser = new Parser(source);
StopNode *ret = parser->parse(true)->iterate();
std::ostringstream builder;
std::string result = ret->print(builder).str();
EXPECT_EQ(result,
"return Phi(Loop9,(flt)arg,(((ToFloat/Phi_guess)+Phi_guess)/2.0))");
}


TEST(SimpleTest, testFPOps
) {
std::string source = R"(
flt x = arg;
return x+1==x;
)";
auto *parser = new Parser(source);
StopNode *ret = parser->parse(true)->iterate();
std::ostringstream builder;
std::string result = ret->print(builder).str();
EXPECT_EQ(result,
"return ((flt)arg==(ToFloat+1.0));");
}
