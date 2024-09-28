#ifndef PARSER_H
#define PARSER_H

#include "../Include/node/add_node.h"
#include "../Include/node/bool_node.h"
#include "../Include/node/constant_node.h"
#include "../Include/node/divnode.h"
#include "../Include/node/if_node.h"
#include "../Include/node/minus_node.h"
#include "../Include/node/mul_node.h"
#include "../Include/node/node.h"
#include "../Include/node/not_node.h"
#include "../Include/node/proj_node.h"
#include "../Include/node/return_node.h"
#include "../Include/node/scope_node.h"
#include "../Include/node/start_node.h"
#include "../Include/node/stop_node.h"
#include "../Include/node/sub_node.h"
#include "../Include/type/integer_type.h"

#include <iostream>
#include <unordered_set>

class Lexer {
public:
  Lexer() = default;

  Lexer(std::string &source);

  bool isEof();

  std::string getAnyNextToken();

  bool matchx(std::string syntax);

  bool match(std::string syntax);

  bool isNumber();

  bool isNumber(char ch);

  bool isPunctuation(char ch);

  std::string parseId();

  std::string parseNumberString();

  Type *parseNumber();

  void skipWhiteSpace();

  std::string get_input();

  bool peek(char ch);

  std::string matchId();

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
  static StartNode *START;
  StopNode *STOP;
  std::vector<ScopeNode *> xScopes;

  explicit Parser(std::string source, TypeInteger *arg);

  explicit Parser(std::string source);

  ~Parser();

  StopNode *parse();

  StopNode *parse(bool show);

  std::string src();

  Node *ctrl();

  Node *ctrl(Node *n);

  bool peek(char ch);

private:
  /**
   LIst of keywords disallowed as identifiers
   */
  const std::unordered_set<std::string> KEYWORDS = {"int", "return"};

  Node *parseStatement();

  Node *parseReturn();

  Node *parseExpression();
  Node *parseIf();

  Node *parseAddition();

  Node *parseMultiplication();

  Node *parseUnary();

  Node *parseComparison();

  Node *parseDecl();

  Node *parsePrimary();

  Node *parseIntegerLiteral();

  Node *showGraph();

  Node *parseBlock();

  void require(std::string syntax);

  Node *require(Node *n, std::string syntax);

  // Require and return an identifier
  std::string requireId();

  Node *parseExpressionStatement();

  void errorSyntax(std::string syntax);

  void error(std::string errorMessage);

  bool match(std::string syntax);

  bool matchx(std::string syntax);

  Lexer *lexer;
  ScopeNode *scope_node;
};

#endif
