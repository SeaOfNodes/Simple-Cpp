#ifndef PARSER_H
#define PARSER_H

#include "../Include/node/start_node.h"
#include "../Include/node/return_node.h"
#include "../Include/node/constant_node.h"
#include "../Include/type/integer_type.h"
#include "../Include/node/add_node.h"
#include "../Include/node/sub_node.h"
#include "../Include/node/mul_node.h"
#include "../Include/node/divnode.h"
#include "../Include/node/minus_node.h"

#include "../Include/node/node.h"
#include <iostream>

class Lexer{
public:
  Lexer() = default;
  Lexer(std::string& source);
  bool isEof();
  std::string getAnyNextToken();
  bool matchx(std::string syntax);
  bool match(std::string syntax);
  bool isNumber();
  bool isNumber(char ch);
  bool isPunctuation(char ch);

  std::string parseId();
  std::string parseNumberString();

  Type* parseNumber();
  void skipWhiteSpace();
  std::string get_input();
private:
  std::string input;
  std::size_t position = 0;
  bool isIdStart(char ch);
  bool isIdLetter(char ch);
  char peek();
  char nextChar();
  bool isWhiteSpace();

  std::string parsePunctuation();

};

class Parser {
public:
  static StartNode* START;
  explicit Parser(std::string source);
  ReturnNode* parse();
  std::string src();
private:
  Node* parseStatement();
  Node* parseReturn();
  Node* parseExpression();
  Node* parseAddition();
  Node* parseMultiplication();
  Node* parseUnary();

  Node* parsePrimary();

  Node* parseIntegerLiteral();

  void require(std::string syntax);

  Node* require(Node* n, std::string syntax);
  void errorSyntax(std::string syntax);
  void error(std::string errorMessage);

  bool match(std::string syntax);
  bool matchx(std::string syntax);

  Lexer* lexer;
};

#endif