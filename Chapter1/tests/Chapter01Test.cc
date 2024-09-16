#include "parser.h"
#include <gtest/gtest.h>
#include <memory>
#include <iostream>

TEST(Simple, SimpleProgram){
  Parser* parser = new Parser("return 1;");
  ReturnNode* ret = parser->parse();
  StartNode* start = Parser::START;
  EXPECT_EQ(start, ret->ctrl());
  Node* expr = ret->expr();

 if (ConstantNode* con = dynamic_cast<ConstantNode*>(expr)) {

   EXPECT_EQ(start, con->in(0));

   EXPECT_EQ(1, con->value_);
 } else {

   FAIL() << "Expression is not a ConstantNode";
 }
}

TEST(Simple, SimpleZero) {
 Parser* parser = new Parser("return 0;");
 parser->parse();
 StartNode* start = Parser::START;
 for (const auto& use: start->outputs){
   if (ConstantNode* con = dynamic_cast<ConstantNode*>(use)) {
     EXPECT_EQ(0, con->value_);
   }
 }
}
TEST(Simple, Bad1){
 std::string expectedMessage = "Syntax error, expected a statement: ret";
 try {
   Parser*parser = new Parser("ret");
   parser->parse();
   FAIL();
 } catch (const std::runtime_error& e){
   EXPECT_EQ(expectedMessage, e.what());
 }
}

TEST(Simple, Bad2){
 std::string expectedMessage = "Syntax error: integer values cannot start with '0'";
   try {
     Parser* parser = new Parser("return 0123;");
     parser->parse();
     FAIL();
   } catch(const std::runtime_error& e ) {
     EXPECT_EQ(expectedMessage,e.what());
   }
}

TEST(Simple, Bad3){
   std::string expectedMessage = "Syntax error, expected integer literal";
   try {
     Parser* parser = new Parser("return --12;");
     parser->parse();
     FAIL();
   } catch(const std::runtime_error& e ) {
     EXPECT_EQ(expectedMessage,e.what());
   }
}

/*
TEST(Simple, Bad4){
   std::string expectedMessage = "Syntax error, expected ;: ";
   try {
     Parser* parser = new Parser("return 100");
     parser->parse();
     FAIL();
   } catch(const std::runtime_error& e ) {
     EXPECT_EQ(expectedMessage,e.what());
   }
}*/


TEST(Simple, Bad7){
   std::string expectedMessage = "Syntax error, unexpected }";
   try {
     Parser* parser = new Parser("return 12;}");
     parser->parse();
     FAIL();
   } catch(const std::runtime_error& e ) {
     EXPECT_EQ(expectedMessage,e.what());
   }
}