#include "../Include/graph_evaluator.h"
#include "../Include/graph_visualizer.h"

#include <gtest/gtest.h>

#include <iostream>
#include <sstream>

//Todo: fix most of these here
// Todo: Ignore jig
/*TEST(SimpleTest, testJig) {
  std::string source = R"(
int v0=0;
arg=0;
while(v0) {
        while(1) if(arg*arg*0==0) {}
                while(0) {}
    arg=1;
}
return 0;
)";
  auto *parser = new Parser(source);
  StopNode *ret = parser->parse(false)->iterate();
  std::ostringstream builder;
  std::string result = ret->print(builder).str();
}*/

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

/*// Todo: still need fix
TEST(SimpleTest, testWorklist1) {
  std::string source = R"(
  int step = 1;
while (arg < 10) {
  arg = arg + step + 1;
}
return arg;
)";
  auto *parser = new Parser(source);
  StopNode *ret = parser->parse()->iterate();
  std::ostringstream builder;
  std::string result = ret->print(builder).str();

  EXPECT_EQ("return Phi(Loop7,arg,(Phi_arg+2));", result);
  EXPECT_EQ(11, GraphEvaluator::evaluate(ret, 1));
}*/


// Todo: Still need fix
TEST(SimpleTest, testWorklist2) {
  std::string source = R"(
int cond = 0;
int one = 1;
while (arg < 10) {
    if (cond) one = 2;
    arg = arg + one*3 + 1;
}
return arg;
)";

  auto *parser = new Parser(source);
  StopNode *ret = parser->parse()->iterate();
  std::ostringstream builder;
  std::string result = ret->print(builder).str();

  EXPECT_EQ("return Phi(Loop8,arg,(Phi_arg+4));", result);
  EXPECT_EQ(13, GraphEvaluator::evaluate(ret, 1));
}

TEST(SimpleTest, testWorklist3) {
  std::string source = R"(
int v1 = 0;
int v2 = 0;
int v3 = 0;
int v4 = 0;
int v5 = 0;
int v6 = 0;
int v7 = 0;
int v8 = 0;
while (arg) {
    if (v1) v2 = 1;
    if (v2) v3 = 1;
    if (v3) v4 = 1;
    if (v4) v5 = 1;
    if (v5) v6 = 1;
    if (v6) v7 = 1;
    if (v7) v8 = 1;
    arg = arg + v8 + 1;
}
return arg;
)";
  auto *parser = new Parser(source);
  StopNode *ret = parser->parse()->iterate();
  std::ostringstream builder;
  std::string result = ret->print(builder).str();

  EXPECT_EQ("return Phi(Loop14,arg,(Phi_arg+1));", result);
}

TEST(SimpleTest, testRegionPeepBug) {
  std::string source = R"(
int v0=0;
int v1=0;
while(v1+arg) {
    arg=0;
    int v2=v0;
    while(arg+1) {}
    v0=1;
    v1=v2;
}
)";
  auto *parser = new Parser(source);
  StopNode *ret = parser->parse()->iterate();
  std::ostringstream builder;
  std::string result = ret->print(builder).str();

  EXPECT_EQ("Stop[ ]", result);
}

TEST(SimpleTest, testWhile0) {
  std::string source = R"(
while(0) continue; if(0) arg=0;
)";
  auto *parser = new Parser(source);
  StopNode *ret = parser->parse()->iterate();
  std::ostringstream builder;
  std::string result = ret->print(builder).str();

  EXPECT_EQ("Stop[ ]", result);
}


TEST(SimpleTest, testWhile1) {
  std::string source = R"(
if(0) while(0) {
    int arg=arg;
    while(0) {}
}
)";
  auto *parser = new Parser(source);
  StopNode *ret = parser->parse()->iterate();
  std::ostringstream builder;
  std::string result = ret->print(builder).str();

  EXPECT_EQ("Stop[ ]", result);
}


TEST(SimpleTest, testPrecedence) {
  std::string source = R"(
return 3-1+2;
)";
  auto *parser = new Parser(source);
  StopNode *ret = parser->parse()->iterate();
  std::ostringstream builder;
  std::string result = ret->print(builder).str();

  EXPECT_EQ("return 4;", result);
}

TEST(SimpleTest, testSwap2) {
  std::string source = R"(
return 1+(1+1);
)";
  auto *parser = new Parser(source);
  StopNode *ret = parser->parse()->iterate();
  std::ostringstream builder;
  std::string result = ret->print(builder).str();

  EXPECT_EQ("return 3;", result);
}

TEST(SimpleTest, testFuzz0) {
  std::string source = R"(
int one = 1;
int a = 0;
int zero = 0;
while(arg) {
    a = -(one + a + 2);
    arg = arg + 1;
    one = one + zero;
}
return a;
)";
  auto *parser = new Parser(source);
  StopNode *ret = parser->parse()->iterate();
  std::ostringstream builder;
  std::string result = ret->print(builder).str();

  EXPECT_EQ("return Phi(Loop9,0,(-(Phi_a+3)));", result);
}

TEST(SimpleTest, testFuzz1) {
  std::string source = R"(
while(1) {}
while(arg) break;
while(arg) arg=0;
arg=0;
int v0=0!=0<-0;
return -0+0+0;
)";
  auto *parser = new Parser(source);
  StopNode *ret = parser->parse()->iterate();
  std::ostringstream builder;
  std::string result = ret->print(builder).str();

  EXPECT_EQ("Stop[]", result);
}


TEST(SimpleTest, testFuzz2) {
  std::string source = R"(
return 0+-0;
)";
  auto *parser = new Parser(source);
  StopNode *ret = parser->parse()->iterate();
  std::ostringstream builder;
  std::string result = ret->print(builder).str();

  EXPECT_EQ("return 0;", result);
}

TEST(SimpleTest, testFuzz3) {
  std::string source = R"(
int v0=0; while(0==69) while(v0) return 0;
)";
  auto *parser = new Parser(source);
  StopNode *ret = parser->parse()->iterate();
  std::ostringstream builder;
  std::string result = ret->print(builder).str();

  EXPECT_EQ("Stop[ ]", result);
}

TEST(SimpleTest, testFuzz4) {
  std::string source = R"(
while(1) {
    arg=0<=0;
    if(1<0) while(arg==-0) arg=arg-arg;
}
)";
  auto *parser = new Parser(source);
  StopNode *ret = parser->parse()->iterate();
  std::ostringstream builder;
  std::string result = ret->print(builder).str();

  EXPECT_EQ("Stop[ ]", result);
}

TEST(SimpleTest, testFuzz5) {
  std::string source = R"(
int v0=0;
while(1)
        int v1=0--0;
while(v0)
    break;
while(-v0) {
    while(0+0+v0) continue;
    break;
}
if(-0!=-0+0+v0) while(0+0+0+0)
            break;
}
return 0!=0;
)";
  auto *parser = new Parser(source);
  StopNode *ret = parser->parse()->iterate();
  std::ostringstream builder;
  std::string result = ret->print(builder).str();

  EXPECT_EQ("Stop[ ]", result);
}

TEST(SimpleTest, testFuzz6) {
  std::string source = R"(
int v0=0;
while(0==1) while(v0)
        v0=1+v0;
)";
  auto *parser = new Parser(source);
  StopNode *ret = parser->parse()->iterate();
  std::ostringstream builder;
  std::string result = ret->print(builder).str();

  EXPECT_EQ("Stop[ ]", result);
}

TEST(SimpleTest, testFuzz7) {
  std::string source = R"(
while(1) {}
int v0=0;
while(v0)
    {}
int v1=0;
while(1)
        v1=1;
return v1+v0;
)";
  auto *parser = new Parser(source);
  StopNode *ret = parser->parse()->iterate();
  std::ostringstream builder;
  std::string result = ret->print(builder).str();

  EXPECT_EQ("Stop[ ]", result);
}

TEST(SimpleTest, testFuzz8) {
  std::string source = R"(
while(arg) arg = arg - 1;  return arg;
)";
  auto *parser = new Parser(source);
  StopNode *ret = parser->parse()->iterate();
  std::ostringstream builder;
  std::string result = ret->print(builder).str();

  EXPECT_EQ("return Phi(Loop6,arg,(Phi_arg-1));", result);
}


TEST(SimpleTest, testMeet) {
  Type*t1 = Type::TOP();
  Type*t2 = TypeInteger::TOP();

  EXPECT_EQ(TypeInteger::TOP(), t1->meet(t2));
  EXPECT_EQ(TypeInteger::TOP(), t2->meet(t1));

  t1 = Type::BOTTOM();
  t2 = TypeInteger::BOT();

  EXPECT_EQ(Type::BOTTOM(), t1->meet(t2));
  EXPECT_EQ(Type::BOTTOM(), t2->meet(t1));
}
