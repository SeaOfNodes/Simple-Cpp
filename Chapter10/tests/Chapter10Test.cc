#include "../Include/graph_evaluator.h"
#include "../Include/graph_visualizer.h"
#include "../Include/IR_printer.h"
#include <gtest/gtest.h>

#include <iostream>
#include <sstream>

// Todo: segfault
TEST(SimpleTest, TestFuzzer) {
    std::string source = R"(
int a = arg/3;
int b = arg*5;
int x = arg*7;
int y = arg/11;
int p; int g; int h;
if( (arg/13)==0 ) {
    p = x + y;
    g = x;
    h = y;
} else {
    p = a + b;
    g = a;
    h = b;
}
int r = g+h;
return p-r;
)";

    auto *parser = new Parser(
            source
    );
    StopNode *ret = parser->parse()->iterate();
    std::ostringstream builder;
    std::string result = ret->print(builder).str();
    EXPECT_EQ("return 0;", result);
}

//TEST(SimpleTest, testStruct) {
//    std::string source = R"(
//struct Bar {
//    int a;
//    int b;
//}
//struct Foo {
//    int x;
//}
//Foo? foo = null;
//Bar bar = new Bar;
//bar.a = 1;
//bar.a = 2;
//return bar.a;
//)";
//
//    auto *parser = new Parser(
//            source
//    );
//    StopNode *ret = parser->parse()->iterate();
//    std::ostringstream builder;
//    std::string result = ret->print(builder).str();
//    EXPECT_EQ("return 2;", result);
//}
//
//TEST(SimpleTest, testExample) {
//    std::string source = R"(
//struct Vector2D { int x; int y; }
//Vector2D v = new Vector2D;
//v.x = 1;
//if (arg)
//    v.y = 2;
//else
//    v.y = 3;
//return v;
//)";
//
//    auto *parser = new Parser(
//            source
//    );
//    StopNode *ret = parser->parse()->iterate();
//    std::ostringstream builder;
//    std::string result = ret->print(builder).str();
//    EXPECT_EQ("return new Vector2D;", result);
//}

// expr.type_ and t dont have correct meet operation
// is a fails, type doesnt improve
//TEST(SimpleTest, testBug) {
//    std::string source = R"(
//struct s0 {
//    int v0;
//}
//s0? v1=null;
//int v3=v1.zAicm;
//)";
//
//    auto *parser = new Parser(
//            source
//    );
//    try {
//        StopNode *ret = parser->parse()->iterate();
//    } catch (std::runtime_error &e) {
//        std::string error = e.what();
//        EXPECT_EQ("Accessing unknown field 'zAicm' from 'NULLPTR'", error);
//    }
//}

//TEST(SimpleTest, testBug2) {
//    std::string source = R"(
//struct s0 { int v0; }
//arg=0+new s0.0;
//)";
//
//    auto *parser = new Parser(
//            source
//    );
//    try {
//        StopNode *ret = parser->parse()->iterate();
//    } catch (std::runtime_error &e) {
//        std::string error = e.what();
//        // Todo: fix this
//        EXPECT_EQ("Expected an identifier, found 'NULLPTR'", error);
//    }
//}
//
//TEST(SimpleTest, testLoop) {
//    std::string source = R"(
//struct Bar { int a; }
//Bar bar = new Bar;
//while (arg) {
//    bar.a = bar.a + 2;
//    arg = arg + 1;
//}
//return bar.a;
//)";
//
//    auto *parser = new Parser(
//            source
//    );
//    StopNode *ret = parser->parse(false)->iterate();
//    std::ostringstream builder;
//    std::string result = ret->print(builder).str();
//    EXPECT_EQ("return Phi(Loop10,0,(Phi_a+2));", result);
//}

//
//TEST(SimpleTest, testIf) {
//    std::string source = R"(
//struct Bar { int a; }
//Bar bar = new Bar;
//if (arg) bar = null;
//bar.a = 1;
//return bar.a;
//)";
//
//    auto *parser = new Parser(
//            source
//    );
//    try {
//        StopNode *ret = parser->parse()->iterate();
//    } catch (std::runtime_error &e) {
//        std::string error = e.what();
//        EXPECT_EQ("Type NULLPTR is not of declared type *Bar", error);
//    }
//}

//TEST(SimpleTest, testIf2) {
//    std::string source = R"(
//struct Bar { int a; }
//Bar? bar = null;
//if (arg) bar = new Bar;
//bar.a = 1;
//return bar.a;
//)";
//
//    auto *parser = new Parser(
//            source
//    );
//    try {
//        StopNode *ret = parser->parse()->iterate();
//    } catch (std::runtime_error &e) {
//        std::string error = e.what();
//        EXPECT_EQ("Might be null accessing 'a'", error);
//    }
//}

//TEST(SimpleTest, testIf3) {
//    std::string source = R"(
//struct Bar { int a; }
//Bar bar = null;
//if (arg) bar = null;
//bar.a = 1;
//return bar.a;
//)";
//
//    auto *parser = new Parser(
//            source
//    );
//    try {
//        StopNode *ret = parser->parse()->iterate();
//    } catch (std::runtime_error &e) {
//        std::string error = e.what();
//        EXPECT_EQ("Type NULLPTR is not of declared type *Bar", error);
//    }
//}

//TEST(SimpleTest, testIfOrNull) {
//    std::string source = R"(
//struct Bar { int a; }
//Bar? bar = new Bar;
//if (arg) bar = null;
//if( bar ) bar.a = 1;
//return bar;
//)";
//
//    auto *parser = new Parser(
//            source
//    );
//    StopNode *ret = parser->parse()->iterate();
//    std::ostringstream builder;
//    std::string result = ret->print(builder).str();
//    EXPECT_EQ("return Phi(Region15,NULLPTR,new Bar);", result);
//}

//TEST(SimpleTest, testIfOrNull2) {
//    std::string source = R"(
//struct Bar { int a; }
//Bar? bar = new Bar;
//if (arg) bar = null;
//int rez = 3;
//if( !bar ) rez=4;
//else bar.a = 1;
//return rez;
//)";
//
//    auto *parser = new Parser(
//            source
//    );
//    StopNode *ret = parser->parse()->iterate();
//    std::ostringstream builder;
//    std::string result = ret->print(builder).str();
//    EXPECT_EQ("return Phi(Region32,4,3);", result);
//}


//TEST(SimpleTest, testWithNullInside) {
//    std::string source = R"(
//struct s0 {int v0;}
//s0? v0 = new s0;
//int ret = 0;
//while(arg) {
//    ret = v0.v0;
//    v0 = null;
//    arg = arg - 1;
//}
//return ret;
//)";
//
//    auto *parser = new Parser(
//            source
//    );
//    try {
//        StopNode *ret = parser->parse()->iterate();
//    } catch (std::runtime_error &e) {
//        std::string error = e.what();
//        EXPECT_EQ("Might be null accessing `v0`", error);
//    }
//}

//TEST(SimpleTest, testRedeclareStruct) {
//    std::string source = R"(
//struct s0 {
//    int v0;
//}
//s0? v1=new s0;
//s0? v1;
//v1=new s0;
//)";
//
//    auto *parser = new Parser(
//            source
//    );
//    try {
//        StopNode *ret = parser->parse()->iterate();
//    } catch (std::runtime_error &e) {
//        std::string error = e.what();
//        EXPECT_EQ("Redefining name `v1`", error);
//    }
//}


//TEST(SimpleTest, testIter) {
//    std::string source = R"(
//struct Iter {
//    int x;
//    int len;
//}
//Iter i = new Iter;
//i.len = arg;
//int sum=0;
//while( i.x < i.len ) {
//    sum = sum + i.x;
//    i.x = i.x + 1;
//}
//return sum;
//)";
//    auto *parser = new Parser(
//            source
//    );
//    StopNode *ret = parser->parse()->iterate();
//    std::ostringstream builder;
//    std::string result = ret->print(builder).str();
//    EXPECT_EQ("return Phi(Loop14,0,(Phi(Loop,0,(Phi_x+1))+Phi_sum));", result);
//
//}

//TEST(SimpleTest, test1) {
//    std::string source = R"(
//struct s0 {int v0;}
//s0 ret = new s0;
//while(arg) {
//    s0 v0 = new s0;
//    v0.v0 = arg;
//    arg = arg-1;
//    if (arg==5) ret=v0;
//}
//return ret;
//)";
//    auto *parser = new Parser(
//            source
//    );
//    StopNode *ret = parser->parse()->iterate();
//    std::ostringstream builder;
//    std::string result = ret->print(builder).str();
//    EXPECT_EQ("return Phi(Loop10,new s0,Phi(Region31,new s0,Phi_ret));", result);
//
//}

//TEST(SimpleTest, test2) {
//    std::string source = R"(
//struct s0 {int v0;}
//s0 ret = new s0;
//s0 v0 = new s0;
//while(arg) {
//    v0.v0 = arg;
//    arg = arg-1;
//    if (arg==5) ret=v0;
//}
//return ret;
//)";
//    auto *parser = new Parser(
//            source
//    );
//    StopNode *ret = parser->parse(false)->iterate();
//    std::ostringstream builder;
////    std::cerr << ret->p(99);
//    std::string result = ret->print(builder).str();
//    EXPECT_EQ("return Phi(Loop13,new s0,Phi(Region32,new s0,Phi_ret));", result);
//
//}

//TEST(SimpleTest, test3) {
//    std::string source = R"(
//struct s0 {int v0;}
//s0 ret = new s0;
//while(arg < 10) {
//    s0 v0 = new s0;
//    if (arg == 5) ret=v0;
//    arg = arg + 1;
//}
//return ret;
//)";
//    auto *parser = new Parser(
//            source
//    );
//    StopNode *ret = parser->parse()->iterate();
//    std::ostringstream builder;
//    std::string result = ret->print(builder).str();
//    EXPECT_EQ("return Phi(Loop10,new s0,Phi(Region30,new s0,Phi_ret));", result);
//
//}
//
//TEST(SimpleTest, testBug3) {
//    std::string source = R"(
//struct s0 {
//    int f0;
//}
//if(0>=0) return new s0;
//return new s0;
//int v0=null.f0;
//)";
//
//    auto *parser = new Parser(
//            source
//    );
//    try {
//        StopNode *ret = parser->parse()->iterate();
//    } catch (std::runtime_error &e) {
//        std::string error = e.what();
//        EXPECT_EQ("Accessing unknown field 'f0' from 'NULLPTR'", error);
//    }
//}

// Type bot is not of declared type _int
TEST(SimpleTest, testBug4) {
    std::string source = R"(
if(0) {
    while(0) if(arg) continue;
    int v0=0;
    while(1) {
        int arg=-arg;
        v0=arg;
    }
}
)";
    auto *parser = new Parser(
            source
    );
    StopNode *ret = parser->parse()->iterate();
    std::ostringstream builder;
    std::string result = ret->print(builder).str();
    EXPECT_EQ("Stop[ ]", result);
}


//TEST(SimpleTest, testBug5) {
//    std::string source = R"(
//struct s0 {
//    int f0;
//}
//if(0) return 0;
//else return new s0;
//if(new s0.f0) return 0;
//)";
//    auto *parser = new Parser(
//            source
//    );
//    StopNode *ret = parser->parse()->iterate();
//    std::ostringstream builder;
//    std::string result = ret->print(builder).str();
//    EXPECT_EQ("return new s0;", result);
//
//}

//TEST(SimpleTest, testBug6MissedWorklist) {
//    std::string source = R"(
//while(0) {}
//int v4=0;
//while(0<arg) {
//    v4=v4+1;
//    while(1) v4=-v4;
//    while(0) arg=-1;
//}
//return 0;
//)";
//    auto *parser = new Parser(
//            source
//    );
//    StopNode *ret = parser->parse()->iterate();
//    std::ostringstream builder;
//    std::string result = ret->print(builder).str();
//    EXPECT_EQ("return 0;", result);
//
//}

//TEST(SimpleTest, testBug7) {
//    std::string source = R"(
//struct s0 {  int f0; }
//s0 v0 = new s0;
//while(v0.f0) {}
//s0 v1 = v0;
//return v1;
//)";
//    auto *parser = new Parser(
//            source
//    );
//    StopNode *ret = parser->parse()->iterate();
//    std::ostringstream builder;
//    std::string result = ret->print(builder).str();
//    EXPECT_EQ("return new s0;", result);
//
//}

//// Todo: segfault
TEST(SimpleTest, testBug8) {
    std::string source = R"(
int v2=0;
while(0)
while(0) {}
{
    {
        {
            int v36=0;
            {
                while(0) {
                    {
                        while(-v2) {
                            {
                                while(v36) {
                                                while(v2) return 0;
                                                break;
                                }                            }
                            if(-v2) break;
                        }
                    }
                }
            }
        }    }
}
)";
    auto *parser = new Parser(
            source
    );
    StopNode *ret = parser->parse(false)->iterate();
    std::ostringstream builder;
    std::string result = ret->print(builder).str();
    EXPECT_EQ("Stop[ ];", result);

}

// the hashmap to wrap and act as a ringbuffer
// investigate the error
//TEST(SimpleTest, testBug9) {
//    std::string source = R"(
//int v0=arg==0;
//while(v0) continue;
//return 0;
//)";
//    auto *parser = new Parser(
//            source
//    );
//    StopNode *ret = parser->parse()->iterate();
//    std::ostringstream builder;
//    std::string result = ret->print(builder).str();
//    EXPECT_EQ("return 0;", result);
//}