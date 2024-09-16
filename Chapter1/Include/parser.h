#ifndef PARSER_H
#define PARSER_H

#include "../Include/node/start_node.h"
#include "../Include/node/return_node.h"
#include "../Include/node/constant_node.h"

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

  long parseNumber();
  void skipWhiteSpace();

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
private:
  Node* parseStatement();
  Node* parseReturn();
  Node* parseExpression();

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