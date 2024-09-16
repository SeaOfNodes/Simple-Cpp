#include "../Include/graph_visualizer.h"
#include "../Include/node/return_node.h"
#include "parser.h"
#include <gtest/gtest.h>
#include <iostream>
#include <sstream>

TEST(SimpleTest, VarDecl) {
  Node::disablePeephole = false;
  auto *parser = new Parser("int a=1; return a;");
  ReturnNode *ret = parser->parse();
  std::ostringstream builder;
  EXPECT_EQ("return 1;", ret->print_1(builder).str());
}

TEST(SimpleTest, VarAdd) {
  Node::disablePeephole = false;
  auto *parser = new Parser("int a = 1; int b = 2; return a+b;");
  ReturnNode *ret = parser->parse();
  std::ostringstream builder;
  EXPECT_EQ("return 3;", ret->print_1(builder).str());
}

TEST(SimpleTest, VarScope) {
  auto *parser = new Parser("int a = 1; int b = 2; return a + b;");
  ReturnNode *ret = parser->parse();
  std::ostringstream builder;
  EXPECT_EQ("return 3;", ret->print_1(builder).str());
}

TEST(SimpleTest, VarScopeNoPeephole) {
  auto *parser = new Parser(
      "int a = 1; int b = 2; int c = 2; {int b = 3; c = a + b;} return c;");
  Node::disablePeephole = true;
  ReturnNode *ret = parser->parse();
  std::ostringstream builder;
  EXPECT_EQ("return (1+3);", ret->print_1(builder).str());
}

TEST(SimpleTest, VarDist) {
  auto *parser = new Parser("int x0=1; int y0=2; int x1=3; int y1=4; return "
                            "(x0-x1)*(x0-x1) + (y0-y1)*(y0-y1);");
  Node::disablePeephole = true;
  ReturnNode *ret = parser->parse();
  std::ostringstream builder;
}

TEST(SimpleTest, SelfAssign) {
  auto *parser = new Parser("int a = a; return a;");
  try {
    parser->parse();
  } catch (std::runtime_error e) {
    std::string a = e.what();
    EXPECT_EQ("Undefined name: 'a'", a);
  }
}

TEST(SimpleTest, Bad1) {
  auto *parser = new Parser("int a=1; int b=2; int c=0; { int b=3; c=a+b;");
  try {
    parser->parse();
  } catch (std::runtime_error e) {
    std::string a = e.what();
    EXPECT_EQ("Syntax error, expected }: ", a);
  }
}
