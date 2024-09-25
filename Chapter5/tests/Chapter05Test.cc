#include "../Include/graph_visualizer.h"
#include "../Include/node/return_node.h"

#include "parser.h"
#include <gtest/gtest.h>

#include <iostream>
#include <sstream>

TEST(SimpleTest, Peephole) {
    auto *parser = new Parser("return 1+arg+2;");
    ReturnNode *ret = parser->parse();
    std::ostringstream builder;
    EXPECT_EQ("return (arg+3);", ret->print_1(builder).str());
}

TEST(SimpleTest, Peephole2) {
    auto *parser = new Parser("return (1+arg)+2;");
    ReturnNode *ret = parser->parse();
    std::ostringstream builder;
    EXPECT_EQ("return (arg+3);", ret->print_1(builder).str());
}

TEST(SimpleTest, ADD0) {
    auto *parser = new Parser("return 0+arg;");
    ReturnNode *ret = parser->parse();
    std::ostringstream builder;
    EXPECT_EQ("return arg;", ret->print_1(builder).str());
}

TEST(SimpleTest, ADDMUL) {
    auto *parser = new Parser("return arg+0+arg;");
    ReturnNode *ret = parser->parse();
    std::ostringstream builder;
    EXPECT_EQ("return (arg*2);", ret->print_1(builder).str());
}

TEST(SimpleTest, Peephole3) {
    auto *parser = new Parser("return 1+arg+2+arg+3;");
    ReturnNode *ret = parser->parse();
    std::ostringstream builder;
    EXPECT_EQ("return ((arg*2)+6);", ret->print_1(builder).str());
}

TEST(SimpleTest, Mul1) {
    auto *parser = new Parser("return 1*arg;");
    ReturnNode *ret = parser->parse();
    std::ostringstream builder;
    EXPECT_EQ("return arg;", ret->print_1(builder).str());
}

TEST(SimpleTest, VarArg) {
    auto *parser = new Parser("return arg;");
    ReturnNode *ret = parser->parse();
    std::ostringstream builder;
    EXPECT_EQ("return arg;", ret->print_1(builder).str());
}

TEST(SimpleTest, ConstantArg) {
    auto *parser = new Parser("return arg; #showGraph;", TypeInteger::constant(2));
    ReturnNode *ret = parser->parse();
    std::ostringstream builder;
    EXPECT_EQ("return 2;", ret->print_1(builder).str());
}

TEST(SimpleTest, CompEQ2) {
    auto *parser = new Parser("return 3==4;");
    ReturnNode *ret = parser->parse();
    std::ostringstream builder;
    EXPECT_EQ("return 0;", ret->print_1(builder).str());
}

TEST(SimpleTest, CompNEq) {
    auto *parser = new Parser("return 3!=3;");
    ReturnNode *ret = parser->parse();
    std::ostringstream builder;
    EXPECT_EQ("return 0;", ret->print_1(builder).str());
}

TEST(SimpleTest, CompNEq2) {
    auto *parser = new Parser("return 3!=4;");
    ReturnNode *ret = parser->parse();
    std::ostringstream builder;
    EXPECT_EQ("return 1;", ret->print_1(builder).str());
}

TEST(SimpleTest, Bug1) {
    auto *parser = new Parser("int a=arg+1; int b=a; b=1; return a+2;");
    ReturnNode *ret = parser->parse();
    std::ostringstream builder;
    EXPECT_EQ("return (arg+3);", ret->print_1(builder).str());
}

TEST(SimpleTest, Bug2) {
    auto *parser = new Parser("int a=arg+1; a=a; return a;");
    ReturnNode *ret = parser->parse();
    std::ostringstream builder;
    EXPECT_EQ("return (arg+1);", ret->print_1(builder).str());
}

TEST(SimpleTest, Bug3) {
    try {
        new Parser("inta=1; return a;");
    } catch (std::runtime_error &e) {
        std::ostringstream builder;
        EXPECT_EQ("Undefined name  'inta'", e.what());
    }
}

TEST(SimpleTest, Bug4) {
    auto *parser = new Parser("return -arg;");
    ReturnNode *ret = parser->parse();
    std::ostringstream builder;
    EXPECT_EQ("return (-arg);", ret->print_1(builder).str());
}
