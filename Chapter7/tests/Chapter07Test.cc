#include "../Include/graph_visualizer.h"
#include "../Include/node/return_node.h"
#include "../Include/node/stop_node.h"

#include <gtest/gtest.h>

#include <iostream>
#include <sstream>

TEST(SimpleTest, testMyOwnSake) {
  std::string source = R"(
  int a = 12;
  int b = 12;
  #showGraph;
 return a + b;
)";
  auto*parser = new Parser(source);
  parser->parse(false);
}

TEST(SimpleTest, testAssignmentSema) {
std::string source = R"(
  int a = 1;
  #showGraph;
  return a;
)";
    auto*parser = new Parser(source);
    parser->parse(false);
}

TEST(SimpleTest, testExample) {
  std::string source = R"(
  while(arg < 10) {
    arg = arg + 1;
    #showGraph;
  }
  return arg;
)";
  auto *parser = new Parser(source);
  Node::disablePeephole = true;
  StopNode*ret = parser->parse(true);
  std::ostringstream builder;
  EXPECT_EQ("return Phi(Loop6,arg,(Phi_arg+1));", ret->print_1(builder).str());
  // something is missing from here
  Node::disablePeephole = false;
}

TEST(SimpleTest, testRegression) {
  std::string source = R"(
  int a = 1;
  if(arg){}else{
    while(a < 10) {
      a = a + 1;
  }
}
return a;
)";
  auto *parser = new Parser(source);
  std::ostringstream builder;
  StopNode*ret = parser->parse(true);
  EXPECT_EQ("return Phi(Region23,1,Phi(Loop11,1,(Phi_a+1)));", ret->print_1(builder).str());
}

TEST(SimpleTest, testWhileNested) {
  std::string source = R"(
int sum = 0;
int i = 0;
while(i < arg) {
    i = i + 1;
    int j = 0;
    while( j < arg ) {
        sum = sum + j;
        j = j + 1;
    }
}
return sum;
)";
  auto *parser = new Parser(source);
  std::ostringstream builder;
  StopNode*ret = parser->parse(true);
  EXPECT_EQ("return Phi(Loop8,0,Phi(Loop21,Phi_sum,(Phi(Loop,0,(Phi_j+1))+Phi_sum)));", ret->print_1(builder).str());
  // IR pretty print
}

TEST(SimpleTest, testWhileScope) {
  std::string source = R"(
int a = 1;
int b = 2;
while(a < 10) {
    if (a == 2) a = 3;
    else b = 4;
}
return b;
)";
  auto *parser = new Parser(source);
  Node::disablePeephole = true;
  std::ostringstream builder;
  StopNode*ret = parser->parse(true);
  Node::disablePeephole = false;
  EXPECT_EQ("return Phi(Loop8,2,Phi(Region27,Phi_b,4));", ret->print_1(builder).str());
  // IR pretty print
}

TEST(SimpleTest, testWhileNestedIfAndInc) {
  std::string source = R"(
int a = 1;
int b = 2;
while(a < 10) {
    if (a == 2) a = 3;
    else b = 4;
    b = b + 1;
    a = a + 1;
}
return b;
)";
  auto *parser = new Parser(source);
  std::ostringstream builder;
  StopNode*ret = parser->parse(true);
  EXPECT_EQ("return Phi(Loop8,2,(Phi(Region27,Phi_b,4)+1));", ret->print_1(builder).str());
  // IR pretty print
}

TEST(SimpleTest, testWhile) {
  std::string source = R"(
int a = 1;
while(a < 10) {
    a = a + 1;
    a = a + 2;
}
return a;
)";
  auto *parser = new Parser(source);
  Node::disablePeephole = true;
  std::ostringstream builder;
  StopNode*ret = parser->parse(true);
  Node::disablePeephole = false;
  EXPECT_EQ("return Phi(Loop7,1,((Phi_a+1)+2));", ret->print_1(builder).str());
  // IR pretty print
}

TEST(SimpleTest, testWhilePeep) {
  std::string source = R"(
int a = 1;
while(a < 10) {
    a = a + 1;
    a = a + 2;
}
return a;
)";
  auto *parser = new Parser(source);
  std::ostringstream builder;
  StopNode*ret = parser->parse(true);
  EXPECT_EQ("return Phi(Loop7,1,(Phi_a+3));", ret->print_1(builder).str());
  // IR pretty print
}

TEST(SimpleTest, testWhile2) {
  std::string source = R"(
int a = 1;
while(arg) a = 2;
return a;
)";
  auto *parser = new Parser(source);
  Node::disablePeephole = true;
  std::ostringstream builder;
  StopNode*ret = parser->parse(true);
  Node::disablePeephole = false;
  EXPECT_EQ("return Phi(Loop7,1,2);", ret->print_1(builder).str());
  // IR pretty print
}

TEST(SimpleTest, testWhile2Peep) {
  std::string source = R"(
int a = 1;
while(arg) a = 2;
return a;
)";
  auto *parser = new Parser(source);
  std::ostringstream builder;
  StopNode*ret = parser->parse(true);
  EXPECT_EQ("return Phi(Loop7,1,2);", ret->print_1(builder).str());
  // IR pretty print
}

TEST(SimpleTest, testWhile3) {
  std::string source = R"(
int a = 1;
while(a < 10) {
    int b = a + 1;
    a = b + 2;
}
return a;
)";
  Node::disablePeephole = true;
  auto *parser = new Parser(source);
  std::ostringstream builder;
  StopNode*ret = parser->parse(true);
  Node::disablePeephole = false;
  EXPECT_EQ("return Phi(Loop7,1,((Phi_a+1)+2));;", ret->print_1(builder).str());
  // IR pretty print
}

TEST(SimpleTest, testWhile3Peep) {
  std::string source = R"(
int a = 1;
while(a < 10) {
    int b = a + 1;
    a = b + 2;
}
return a;
)";
  auto *parser = new Parser(source);
  std::ostringstream builder;
  StopNode*ret = parser->parse(true);
  EXPECT_EQ("return Phi(Loop7,1,(Phi_a+3));", ret->print_1(builder).str());
  // IR pretty print
}

TEST(SimpleTest, testWhile4) {
  std::string source = R"(
int a = 1;
int b = 2;
while(a < 10) {
    int b = a + 1;
    a = b + 2;
}
return a;
)";
  Node::disablePeephole = true;
  auto *parser = new Parser(source);
  std::ostringstream builder;
  StopNode*ret = parser->parse(true);
  EXPECT_EQ("return Phi(Loop8,1,((Phi_a+1)+2));", ret->print_1(builder).str());
  Node::disablePeephole = false;
  // IR pretty print
}
TEST(SimpleTest, testWhile4Peep) {
  std::string source = R"(
int a = 1;
int b = 2;
while(a < 10) {
    int b = a + 1;
    a = b + 2;
}
return a;
)";
  auto *parser = new Parser(source);
  std::ostringstream builder;
  StopNode*ret = parser->parse(true);
  EXPECT_EQ("return Phi(Loop8,1,(Phi_a+3));", ret->print_1(builder).str());
  // IR pretty print
}


