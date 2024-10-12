#include "../Include/graph_visualizer.h"
#include "../Include/node/return_node.h"
#include "../Include/node/stop_node.h"

#include <gtest/gtest.h>

#include <iostream>
#include <sstream>

TEST(SimpleTest, testPeepholeReturn) {
  std::string source = R"(
  if(true) return 2;
  return 1;
)";

  auto *parser = new Parser(source);
  StopNode *ret = parser->parse(false);
  std::ostringstream builder;
  EXPECT_EQ("return 2;", ret->print_1(builder).str());
}

TEST(SimpleTest, testPeepholeRotate) {
  std::string source = R"(
int a = 1;
if (arg)
a = 2;
return (arg < a) < 3;
)";
  auto *parser = new Parser(source, &TypeInteger::BOT);
  StopNode *ret = parser->parse(false);
  std::ostringstream builder;
  EXPECT_EQ("return ((arg<Phi(Region12,2,1))<3);", ret->print_1(builder).str());
}

TEST(SimpleTest, testPeepholeCFG) {
  std::string source = R"(
int a=1;
if( true )
  a=2;
else
  a=3;
return a;
)";
  auto *parser = new Parser(source, &TypeInteger::BOT);
  StopNode *ret = parser->parse(false);
  std::ostringstream builder;
  EXPECT_EQ("return 2;", ret->print_1(builder).str());
}

TEST(SimpleTest, testIfIf) {
  std::string source = R"(
int a=1;
if( arg!=1 )
    a=2;
else
    a=3;
int b=4;
if( a==2 )
    b=42;
else
    b=5;
return b;
)";
  auto *parser = new Parser(source, &TypeInteger::BOT);

  StopNode *ret = parser->parse(false);
  std::ostringstream builder;

  EXPECT_EQ("return Phi(Region32,42,5);", ret->print_1(builder).str());
}

TEST(SimpleTest, testIfArgIf) {
  std::string source = R"(
int a=1;
if( 1==1 )
    a=2;
else
  a=3;
int b=4;
if( arg==2 )
  b=a;
else
  b=5;
return b;
)";
  auto *parser = new Parser(source);

  StopNode *ret = parser->parse(false);
  std::ostringstream builder;
  // id off by one
  EXPECT_EQ("return Phi(Region28,2,5);", ret->print_1(builder).str());
}

TEST(SimpleTest, testMerge3With2) {
  std::string source = R"(
int a=1;
if( arg==1 )
    if( arg==2 )
        a=2;
    else
        a=3;
else if( arg==3 )
    a=4;
else
    a=5;
return a;
)";
  Parser *parser = new Parser(source, TypeInteger::constant(2));
  StopNode *ret = parser->parse(false);
  std::ostringstream builder;
  // id off by one
  EXPECT_EQ("return 5;", ret->print_1(builder).str());
}

TEST(SimpleTest, testMerge3Peephole) {
  std::string source = R"(
int a=1;
if( arg==1 )
    if(1==2 )
        a=2;
    else
        a=3;
else if( arg==3 )
    a=4;
else
    a=5;
return a;
)";
  Parser *parser = new Parser(source, &TypeInteger::BOT);
  StopNode *ret = parser->parse(false);
  std::ostringstream builder;
  EXPECT_EQ("return Phi(Region36,3,Phi(Region34,4,5));",
            ret->print_1(builder).str());
}

TEST(SimpleTest, testMerge3Peephole1) {
  std::string source = R"(
int a=1;
if( arg==1 )
    if( 1==2 )
        a=2;
    else
        a=3;
else if( arg==3 )
    a=4;
else
    a=5;
return a;
)";
  Parser *parser = new Parser(source, TypeInteger::constant(1));
  StopNode *ret = parser->parse(false);
  std::ostringstream builder;
  EXPECT_EQ("return 3;", ret->print_1(builder).str());
}

TEST(SimpleTest, testMerge3Peephole3) {
  std::string source = R"(
int a=1;
if( arg==1 )
    if( 1==2 )
        a=2;
    else
        a=3;
else if( arg==3 )
    a=4;
else
    a=5;
return a;
)";
  Parser *parser = new Parser(source, TypeInteger::constant(3));
  StopNode *ret = parser->parse(false);
  std::ostringstream builder;
  EXPECT_EQ("return 4;", ret->print_1(builder).str());
}

TEST(SimpleTest, testDemo1NonConst) {
  std::string source = R"(
int a = 0;
int b = 1;
if( arg ) {
    a = 2;
    if( arg ) { b = 2; }
    else b = 3;
}
return a+b;
)";
  Parser *parser = new Parser(source);
  StopNode *ret = parser->parse(false);
  std::ostringstream builder;
  EXPECT_EQ("return Phi(Region22,4,1);", ret->print_1(builder).str());
}

TEST(SimpleTest, testDemo1True) {
  std::string source = R"(
int a = 0;
int b = 1;
if( arg ) {
    a = 2;
    if( arg ) { b = 2; }
    else b = 3;
}
return a+b;
)";
  Parser *parser = new Parser(source, TypeInteger::constant(1));
  StopNode *ret = parser->parse(false);
  std::ostringstream builder;
  EXPECT_EQ("return 4;", ret->print_1(builder).str());
}

TEST(SimpleTest, testDemo1False) {
  std::string source = R"(
int a = 0;
int b = 1;
if( arg ) {
    a = 2;
    if( arg ) { b = 2; }
    else b = 3;
}
return a+b;
)";
  Parser *parser = new Parser(source, TypeInteger::constant(0));
  StopNode *ret = parser->parse(false);
  std::ostringstream builder;
  EXPECT_EQ("return 1;", ret->print_1(builder).str());
}

TEST(SimpleTest, testDemo2NonConst) {
  std::string source = R"(
int a = 0;
int b = 1;
int c = 0;
if( arg ) {
    a = 1;
    if( arg==2 ) { c=2; } else { c=3; }
    if( arg ) { b = 2; }
    else b = 3;
}
return a+b+c;
)";
  Parser *parser = new Parser(source);
  StopNode *ret = parser->parse(false);
  std::ostringstream builder;
  EXPECT_EQ("return (Phi(Region33,Phi(Region22,2,3),0)+Phi(Region33,3,1));",
            ret->print_1(builder).str());
}

TEST(SimpleTest, testDemo2True) {
  std::string source = R"(
int a = 0;
int b = 1;
int c = 0;
if( arg ) {
    a = 1;
    if( arg==2 ) { c=2; } else { c=3; }
    if( arg ) { b = 2; }
    else b = 3;
}
return a+b+c;
)";
  Parser *parser = new Parser(source, TypeInteger::constant(1));
  StopNode *ret = parser->parse(false);
  std::ostringstream builder;
  EXPECT_EQ("return 6;", ret->print_1(builder).str());
}

TEST(SimpleTest, testDemo2arg2) {
  std::string source = R"(
int a = 0;
int b = 1;
int c = 0;
if( arg ) {
    a = 1;
    if( arg==2 ) { c=2; } else { c=3; }
    if( arg ) { b = 2; }
    else b = 3;
}
return a+b+c;
)";
  Parser *parser = new Parser(source, TypeInteger::constant(2));
  StopNode *ret = parser->parse(false);
  std::ostringstream builder;
  EXPECT_EQ("return 5;", ret->print_1(builder).str());
}
