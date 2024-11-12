#include "../Include/graph_evaluator.h"
#include "../Include/graph_visualizer.h"

#include <gtest/gtest.h>

#include <iostream>
#include <sstream>

// Todo: Add testJig
TEST(SimpleTest, testEx6) {
  std::string source = R"(
  while(arg < 10) {
  arg = arg + 1;
  if (arg == 5)
      break;
  if (arg == 6)
      break;
}
return arg;
)";
  auto *parser = new Parser(source);
  StopNode *ret = parser->parse(false);
  std::ostringstream builder;
  std::string result = ret->print(builder).str();
  EXPECT_EQ(
      "return Phi(Region36,Phi(Region25,Phi(Loop6,arg,(Phi_arg+1)),Add),Add);",
      result);
}

TEST(SimpleTest, testGVN1) {
  std::string source = R"(
    int x = arg + arg;
if(arg < 10) {
return arg + arg;
}
else {
x = x + 1;
}
return x;
)";
  auto *parser = new Parser(source);
  StopNode *ret = parser->parse();
  std::ostringstream builder;
  std::string result = ret->print(builder).str();

  EXPECT_EQ("Stop[ return (arg*2); return (Mul+1); ]", result);
  EXPECT_EQ(2, GraphEvaluator::evaluate(ret, 1));
  EXPECT_EQ(23, GraphEvaluator::evaluate(ret, 11));
}

TEST(SimpleTest, testGVN2) {
  std::string source = R"(
  return arg*arg-arg*arg;
)";
  auto *parser = new Parser(source);
  StopNode *ret = parser->parse();
  std::ostringstream builder;
  std::string result = ret->print(builder).str();

  EXPECT_EQ("return 0;", result);
  EXPECT_EQ(0, GraphEvaluator::evaluate(ret, 1));
}
