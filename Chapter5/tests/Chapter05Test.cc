#include "../Include/graph_visualizer.h"
#include "../Include/node/return_node.h"
#include "../Include/node/stop_node.h"

#include <gtest/gtest.h>

#include <iostream>
#include <sstream>
TEST(SimpleTest, testIfStmt) {
  std::string source = R"(
  int a = 1;
  if (arg == 1)
     a = arg + 2;
   else {
    a = arg - 3;
}
return a;
)";

  auto *parser = new Parser(source);
  StopNode *ret = parser->parse(false);
  std::ostringstream builder;
  EXPECT_EQ("return Phi(Region16,(arg+2),(arg-3));",
            ret->print_1(builder).str());
}

TEST(SimpleTest, testTest) {
  std::string source = R"(
int c = 3;
int b = 2;
if (arg == 1) {
  b = 3;
  c = 4;
}
return c;
)";
  auto *parser = new Parser(source, &TypeInteger::BOT);
  StopNode *ret = parser->parse(false);
  std::ostringstream builder;
  // id off by one
  EXPECT_EQ("return Phi(Region15,4,3);", ret->print_1(builder).str());
}

TEST(SimpleTest, testReturn2) {
  std::string source = R"(
if( arg==1 )
    return 3;
else
    return 4;
)";
  auto *parser = new Parser(source, &TypeInteger::BOT);
  StopNode *ret = parser->parse(false);
  std::ostringstream builder;
  EXPECT_EQ("Stop[return 3; return 4; ]", ret->print_1(builder).str());
}

TEST(SimpleTest, testMulBy2) {
  std::string source = R"(
    int c = 1;
    int b = c;
    return b+b;
)";
  auto *parser = new Parser(source);
  StopNode *ret = parser->parse(false);
  std::ostringstream builder;

  EXPECT_EQ("return 2;", ret->print_1(builder).str());
}

TEST(SimpleTest, testX) {
  std::string source = R"(
    int a=arg+1;
    if (arg) a=arg+2;
    return a;
)";
  auto *parser = new Parser(source);

  StopNode *ret = parser->parse(false);
  std::ostringstream builder;
  // id off by one
  EXPECT_EQ("return (arg+Phi(Region13,2,1));", ret->print_1(builder).str());
}
// Todo: still not correct
/*
TEST(SimpleTest, testIfMergeB) {
  std::string source = R"(
int a=arg+1;
int b=0;
if(arg==1 )
   b=a;
else
   b=a+1;
return a+b;
)";
  auto *parser = new Parser(source);
  StopNode *ret = parser->parse(false);
  std::ostringstream builder;

  // id off by one
  EXPECT_EQ("return ((arg*2)+Phi(Region19,2,3));", ret->print_1(builder).str());
}
*/

/*
 * a = phi(arg+1, arg+3) = arg+phi(1,3), b = phi(arg*2+3,
 * arg+2) = phi(arg*2, arg) + phi(3,2) and
 * then a+b = arg + phi(1,3) + phi(arg*2, arg) + phi(3,2) =
 * phi(arg*2, arg) + arg + phi(4, 5)
 */

TEST(SimpleTest, testifMerge2) {
  std::string source = R"(
    int a=arg+1;
    int b=arg+2;
    if( arg==1 )
      b=b+a;
  else
    a=b+1;
return a+b;
)";
  // Id off by one
  auto *parser = new Parser(source);
  StopNode *ret = parser->parse(false);
  std::ostringstream builder;
  EXPECT_EQ("return ((Phi(Region30,(arg*2),arg)+arg)+Phi(Region30,4,5));",
            ret->print_1(builder).str());
}

TEST(SimpleTest, testIfMerge3) {
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
  auto *parser = new Parser(source, &TypeInteger::BOT);
  StopNode *ret = parser->parse(false);
  std::ostringstream builder;

  EXPECT_EQ("return Phi(Region30,Phi(Region19,2,3),Phi(Region28,4,5));",
            ret->print_1(builder).str());
}
TEST(SimpleTest, testIfMerge4) {
  std::string source = R"(
int a=0;
int b=0;
if( arg )
  a=1;
if( arg==0 )
  b=2;
return arg+a+b;
)";
  auto *parser = new Parser(source, &TypeInteger::BOT);
  StopNode *stop = parser->parse(false);
  std::ostringstream builder;
  EXPECT_EQ("return ((arg+Phi(Region12,1,0))+Phi(Region20,2,0));",
            stop->print_1(builder).str());
}
TEST(SimpleTest, testIfMerge5) {
  std::string source = R"(
int a=arg==2;
if( arg==1 )
{
  a=arg==3;
}
return a;
)";
  auto *parser = new Parser(source);

  StopNode *ret = parser->parse(false);
  std::ostringstream builder;
  EXPECT_EQ("return (arg==Phi(Region15,3,2));", ret->print_1(builder).str());
}

TEST(SimpleTest, testIfTrue) {
  StopNode *stop = (new Parser("return true;"))->parse(false);
  std::ostringstream builder;
  EXPECT_EQ("return 1;", stop->print_1(builder).str());
}

TEST(SimpleTest, testHalfDef) {
  try {
    (new Parser("if( arg==1 ) int b=2; return b;"))->parse();
  } catch (std::runtime_error &e) {
    std::string error = e.what();
    EXPECT_EQ("Cannot define a new name on one arm of an if", error);
  }
}

TEST(SimpleTest, testHalfDe2) {
  try {
    (new Parser("if( arg==1 ) { int b=2; } else { int b=3; } return b;"))
        ->parse();
  } catch (std::runtime_error &e) {
    std::string error = e.what();
    EXPECT_EQ("Undefined name: 'b'", error);
  }
}

TEST(SimpleTest, testRegress1) {
  try {
    (new Parser("if(arg==2) int a=1; else int b=2; return a;"))->parse();
  } catch (std::runtime_error &e) {
    std::string error = e.what();
    EXPECT_EQ("Cannot define a new name on one arm of an if", error);
  }
}

TEST(SimpleTest, testBadNum) {
  try {
    (new Parser("return 1-;"))->parse();
  } catch (std::runtime_error &e) {
    std::string error = e.what();
    EXPECT_EQ("Syntax error, expected an identifier or expression: ;", error);
  }
}

TEST(SimpleTest, testKeyword1) {
  try {
    (new Parser("int true=0; return true;"))->parse();
  } catch (std::runtime_error &e) {
    std::string error = e.what();
    EXPECT_EQ("Expected an identifier, found true", error);
  }
}

TEST(SimpleTest, testKeyword2) {
  try {
    (new Parser("int else=arg; if(else) else=2; else else=1; return else;"))
        ->parse();
  } catch (std::runtime_error &e) {
    std::string error = e.what();
    EXPECT_EQ("Expected an identifier, found else", error);
  }
}

TEST(SimpleTest, testKeyword3) {
  try {
    (new Parser("int a=1; ififif(arg)inta=2;return a;"))->parse();
  } catch (std::runtime_error &e) {
    std::string error = e.what();
    EXPECT_EQ("Syntax error, expected =: (", error);
  }
}
