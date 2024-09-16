#include "../Include/parser.h"
#include <cctype>

StartNode* Parser::START = nullptr;

Parser::Parser(std::string source) {
  lexer = new Lexer(source);
  Node::reset();
  START = new StartNode();
}

ReturnNode *Parser::parse() {
  auto *ret = dynamic_cast<ReturnNode *>(parseStatement());
  if (!lexer->isEof())
    throw std::runtime_error("Syntax error, unexpected " +
                             lexer->getAnyNextToken());
  return ret;
}

Node *Parser::parseStatement() {
  if (matchx("return"))
    return parseReturn();
  throw std::runtime_error("Syntax error, expected a statement: ret");
}

Node *Parser::parseReturn() {
  Node *expr = require(parseExpression(), ";");
  return new ReturnNode(START, expr);
}

Node *Parser::parseIntegerLiteral() {
  return new ConstantNode(lexer->parseNumber(), START);
}

long Lexer::parseNumber() {
  std::string snum = parseNumberString();
  if (snum.length() > 1 && snum[0] == '0')
    throw std::runtime_error(
        "Syntax error: integer values cannot start with '0'");
  return std::stol(snum);
}

Node *Parser::parseExpression() { return parsePrimary(); }
Node *Parser::parsePrimary() {
  lexer->skipWhiteSpace();
  if (lexer->isNumber())
    return parseIntegerLiteral();
  throw std::runtime_error("Syntax error, expected integer literal");
}

void Parser::require(std::string syntax) { require(nullptr, syntax); }

Node *Parser::require(Node *n, std::string syntax) {
  if (match(syntax))
    return n;
  errorSyntax(syntax);
}

void Parser::errorSyntax(std::string syntax) {
  return error("Syntax error, expected " + syntax + ": " +
               lexer->getAnyNextToken());
}
void Parser::error(std::string errorMessage) {
  throw std::runtime_error(errorMessage);
}
bool Parser::match(std::string syntax) { return lexer->match(syntax); }

bool Parser::matchx(std::string syntax) { return lexer->matchx(syntax); }
void error(std::string errorMessage) { throw std::runtime_error(errorMessage); }

// Lexer
Lexer::Lexer(std::string& source) :input(source){

}
bool Lexer::isEof() { return position >= input.length(); }

char Lexer::peek() { return input[position]; }
char Lexer::nextChar() {
  char ch = peek();
  position++;
  return ch;
}
bool Lexer::isIdStart(char ch) { return (isalpha(ch) != 0) || ch == '_'; }

bool Lexer::isIdLetter(char ch) { return isIdStart(ch) || isdigit(ch); }

bool Lexer::matchx(std::string syntax) {
  if (!match(syntax))
    return false;
  if (!isIdLetter(peek()))
    return true;
  position -= syntax.length();
  return false;
}
bool Lexer::match(std::string syntax) {
  skipWhiteSpace();
  std::size_t len = syntax.length();
  if (position + len > input.length())
    return false;
  for (int i = 0; i < len; i++) {
    if (input[position + i] != syntax[i])
      return false;
  }
  position += len;
  return true;
}
bool Lexer::isNumber() { return isNumber(peek()); }
bool Lexer::isNumber(char ch) { return isdigit(ch); }
bool Lexer::isPunctuation(char ch) {
  return std::string("=;[]<>(){}+-/*!").find(ch) != std::string::npos;
}
std::string Lexer::getAnyNextToken() {
  if (isEof())
    return "";
  if (isIdStart(peek()))
    return parseId();
  if (isNumber(peek()))
    return parseNumberString();
  if (isPunctuation(peek()))
    return parsePunctuation();
}

std::string Lexer::parseId() {
  std::size_t start = position;
  while (isIdLetter(nextChar()))
    ;
  return std::string(input, start, --position - start);
}

std::string Lexer::parsePunctuation() {
  std::size_t start = position;
  return std::string(input, start, 1);
}
std::string Lexer::parseNumberString() {
  std::size_t start = position;
  while (isNumber(nextChar()))
    ;
  return std::string(input, start, --position - start);
}

bool Lexer::isWhiteSpace() { return peek() <= ' '; }
void Lexer::skipWhiteSpace() {
  while (isWhiteSpace())
    position++;
}
