#include "graph_visualizer.h"
#include "parser.h"
#include <gtest/gtest.h>
#include <iostream>
#include <sstream>

TEST(SimpleTest, testParseGrammar) {
  Node::disablePeephole = true;
  auto *parser = new Parser("return 1+2*3+-5;");
  ReturnNode *ret = parser->parse();
  std::ostringstream builder;
  EXPECT_EQ("return (1+((2*3)+(-5)));", ret->print_1(builder).str());
  // Graph visualiser
  auto* gv = new GraphVisualizer();
  std::cout << gv->generateDotOutput(*parser).str();
  Node::disablePeephole = false;
}

TEST(SimpleTest, AddPeephole) {
  Node::disablePeephole = false;
  auto *parser = new Parser("return 1+2;");
  ReturnNode *ret = parser->parse();
  std::ostringstream builder;
  EXPECT_EQ("return 3;", ret->print_1(builder).str());
  auto* gv = new GraphVisualizer();
  std::cout << gv->generateDotOutput(*parser).str();
}

TEST(SimpleTest, SubPeephole) {
  auto *parser = new Parser("return 1-2;");
  ReturnNode *ret = parser->parse();
  std::ostringstream builder;
  EXPECT_EQ("return 3;", ret->print_1(builder).str());
}

TEST(SimpleTest, MulPeephole) {
  auto *parser = new Parser("return 2*3;");
  ReturnNode *ret = parser->parse();
  std::ostringstream builder;
  EXPECT_EQ("return 6;", ret->print_1(builder).str());
}

TEST(SimpleTest, DivPeephole) {
  auto *parser = new Parser("return 6/3;");
  ReturnNode *ret = parser->parse();
  std::ostringstream builder;
  EXPECT_EQ("return 2;", ret->print_1(builder).str());
}

TEST(SimpleTest, MinusPeephole) {
  auto *parser = new Parser("return 6/-3;");
  ReturnNode *ret = parser->parse();
  std::ostringstream builder;
  EXPECT_EQ("return -2;", ret->print_1(builder).str());
}

TEST(SimpleTest, testExample) {
  auto *parser = new Parser("return 1+2*3+-5;");
  ReturnNode *ret = parser->parse();
  std::ostringstream builder;
  EXPECT_EQ("return 2;", ret->print_1(builder).str());
}

