#include "../Include/graph_visualizer.h"

#include <gtest/gtest.h>

#include <iostream>
#include <sstream>

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
  // graph eval here
}

TEST(SimpleTest, testEx5) {
  std::string source = R"(
  int a = 1;
while(arg < 10) {
  arg = arg + 1;
  if (arg == 5)
      continue;
  if (arg == 7)
      continue;
  a = a + 1;
}
return a;
)";
  auto *parser = new Parser(source);
  StopNode *ret = parser->parse(false);
  std::ostringstream builder;
  std::string result = ret->print(builder).str();
  EXPECT_EQ("return Phi(Loop7,1,Phi(Region42,Phi_a,(Phi_a+1)));", result);
  // graph eval here
}
TEST(SimpleTest, testEx4) {
  std::string source = R"(
while(arg < 10) {
    arg = arg + 1;
    if (arg == 5)
        continue;
    if (arg == 6)
        break;
}
return arg;
)";
  auto *parser = new Parser(source);
  StopNode *ret = parser->parse(false);
  std::ostringstream builder;
  std::string result = ret->print(builder).str();
  EXPECT_EQ("return Phi(Region34,Phi(Loop6,arg,(Phi_arg+1)),Add);", result);
  // graph eval here
}

TEST(SimpleTest, testEx3) {
  std::string source = R"(
while(arg < 10) {
    arg = arg + 1;
    if (arg == 6)
        break;
}
return arg;
)";
  auto *parser = new Parser(source);
  StopNode *ret = parser->parse(false);
  std::ostringstream builder;
  std::string result = ret->print(builder).str();
  EXPECT_EQ("return Phi(Region25,Phi(Loop6,arg,(Phi_arg+1)),Add);", result);
  // graph eval here
}

TEST(SimpleTest, testEx2) {
  std::string source = R"(
while(arg < 10) {
    arg = arg + 1;
    if (arg == 5)
        continue;
    if (arg == 6)
        continue;
}
return arg;
)";
  auto *parser = new Parser(source);
  StopNode *ret = parser->parse(false);
  std::ostringstream builder;
  std::string result = ret->print(builder).str();
  EXPECT_EQ("return Phi(Loop6,arg,(Phi_arg+1));", result);
  // graph eval here
}

TEST(SimpleTest, testEx1) {
  std::string source = R"(
while(arg < 10) {
    arg = arg + 1;
    if (arg == 5)
        continue;
}
return arg;
)";
  auto *parser = new Parser(source);
  StopNode *ret = parser->parse(false);
  std::ostringstream builder;
  std::string result = ret->print(builder).str();
  EXPECT_EQ("return Phi(Loop6,arg,(Phi_arg+1));", result);
  // graph eval here
}

TEST(SimpleTest, testRegress1) {
  std::string source = R"(
  while( arg < 10 ) {
  int a = arg+2;
  if( a > 4 )
      break;
}
return arg;
)";
  auto *parser = new Parser(source);
  StopNode *ret = parser->parse(false);
  std::ostringstream builder;
  std::string result = ret->print(builder).str();
  EXPECT_EQ("return arg;", result);
  // graph eval here
}

TEST(SimpleTest, testRegress2) {
  std::string source = R"(
if(1) return 0;  else while(arg>--arg) arg=arg+1; return 0;
)";
  auto *parser = new Parser(source);
  StopNode *ret = parser->parse(false);
  std::ostringstream builder;
  std::string result = ret->print(builder).str();
  EXPECT_EQ("Stop[ return 0; return 0; ]", result);
  // graph eval here
}

TEST(SimpleTest, testBreakOutsideLoop) {
  std::string source = R"(
  if(arg <= 10) {
  break;
  arg = arg + 1;
}
return arg;
)";
  auto *parser = new Parser(source);
  try {
    StopNode *ret = parser->parse(false);
  } catch (std::runtime_error &e) {
    std::string error = e.what();
    EXPECT_EQ("No active loop for a break or continue", error);
  }
  // graph eval here
  // handle exception here
}

TEST(SimpleTest, testRegress3) {
  std::string source = R"(
while(arg < 10) {
    break;
}
return arg;
)";
  auto *parser = new Parser(source);
  StopNode *ret = parser->parse(false);
  std::ostringstream builder;
  std::string result = ret->print(builder).str();
  EXPECT_EQ("return arg;", result);
  // graph eval here
  // handle exception here
}

TEST(SimpleTest, testRegress4) {
  std::string source = R"(
int a = 1;
while(arg < 10) {
    a = a + 1;
    if (arg > 2) {
        int a = 17;
        break;
    }
}
return a;
)";
  auto *parser = new Parser(source);
  StopNode *ret = parser->parse(false);
  std::ostringstream builder;
  std::string result = ret->print(builder).str();
  EXPECT_EQ("return Phi(Region28,Phi(Loop7,1,(Phi_a+1)),Add);", result);
  // graph eval here
}

TEST(SimpleTest, testRegress5) {
  std::string source = R"(
int a = 1;
while(1) {
    a = a + 1;
    if (a<10) continue;
    break;
}
return a;
)";
  auto *parser = new Parser(source);
  StopNode *ret = parser->parse(false);
  std::ostringstream builder;
  std::string result = ret->print(builder).str();
  EXPECT_EQ("return (Phi(Loop7,1,Add)+1);", result);
  // graph eval here
}
