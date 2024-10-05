#include "../Include/parser.h"
#include <cctype>
#include <graph_visualizer.h>
#include <node/proj_node.h>

StartNode *Parser::START = nullptr;

Parser::Parser(std::string source, TypeInteger *arg) {
  Node::reset();
  lexer = new Lexer(source);
  scope_node = new ScopeNode();
  START = new StartNode({&Type::CONTROL, arg});
  STOP = new StopNode({});

  START->peephole();
}

Parser::Parser(std::string source) : Parser(source, &TypeInteger::BOT) {}

Parser::~Parser() {
  delete lexer;
  delete scope_node;
  delete START;
}

StopNode *Parser::parse() { return parse(true); }

StopNode *Parser::parse(bool show) {
  xScopes.push_back(scope_node);
  scope_node->push();
  scope_node->define(ScopeNode::CTRL,
                     (new ProjNode(START, 0, ScopeNode::CTRL))->peephole());
  scope_node->define(ScopeNode::ARG0,
                     (new ProjNode(START, 1, ScopeNode::ARG0))->peephole());
  parseBlock();
  scope_node->pop();
  xScopes.pop_back();

  if (!lexer->isEof())
    throw std::runtime_error("Syntax error, unexpected " +
                             lexer->getAnyNextToken());
  STOP->peephole();
  if (show)
    showGraph();

  return STOP;
}

std::string Parser::src() { return lexer->get_input(); }

Node *Parser::parseStatement() {
  if (matchx("return"))
    return parseReturn();
  else if (matchx("int"))
    return parseDecl();
  else if (match("{"))
    return require(parseBlock(), "}");
  else if (matchx("if"))
    return parseIf();
  else if (matchx("#showGraph"))
    return require(showGraph(), ";");
  else if (match(";"))
    return nullptr;
  else
    return parseExpressionStatement();
}

Node *Parser::parseIf() {
  require("(");
  // Parse predicate
  Node *pred = require(parseExpression(), ")");
  // IfNode takes current control and predicate
  auto *ifNode = (IfNode *)(new IfNode(ctrl(), pred))->keep()->peephole();
  // Setup projection nodes
  Node *ifT = (new ProjNode(ifNode, 0, "True"))->peephole();
  ifNode->unkeep();
  Node *ifF = (new ProjNode(ifNode, 1, "False"))->peephole();
  // In if true branch, the ifT proj node becomes the ctrl
  // But first clone the scope and set it as current

  std::size_t ndefs = scope_node->nIns();
  ScopeNode *fScope = scope_node->dup(); // Duplicate current scope
  xScopes.push_back(fScope); // For graph visualisation we need all scopes

  // Parse the true side
  ctrl(ifT);        // set ctrl token to ifTrue projection
  parseStatement(); // Parse true-side
  ScopeNode *tScope = scope_node;

  scope_node = fScope;
  ctrl(ifF);
  if (matchx("else")) {
    parseStatement();
    fScope = scope_node;
  }

  if (tScope->nIns() != ndefs || fScope->nIns() != ndefs) {
    throw std::runtime_error("Cannot define a new name on one arm of an if");
  }
  scope_node = tScope;
  xScopes.pop_back(); // Discard pushed from graph display

  // Merge scope here
  return ctrl(tScope->mergeScopes(fScope));
}
Node *Parser::showGraph() {
  std::cout << GraphVisualizer().generateDotOutput(*this);
  return nullptr;
}

Node *Parser::parseBlock() {
  // Enter a new scope
  scope_node->push();
  Node *n = nullptr;
  while (!(lexer->peek('}')) && !(lexer->isEof())) {
    Node *n0 = parseStatement();
    if (n0 != nullptr)
      n = n0;
  }
  scope_node->pop();
  return n;
}

Node *Parser::parseExpressionStatement() {
  std::string name = requireId();
  require("=");
  auto expr = require(parseExpression(), ";");
  if (scope_node->update(name, expr) == nullptr)
    error("Undefined name: '" + name + "'");
  return expr;
}

Node *Parser::parseReturn() {
  Node *expr = require(parseExpression(), ";");
  auto *ret = STOP->addReturn(new ReturnNode(ctrl(), expr))->peephole();
  ctrl((new ConstantNode(&Type::XCONTROL, Parser::START))->peephole());
  return ret;
}

Node *Parser::ctrl() { return scope_node->ctrl(); }

Node *Parser::ctrl(Node *n) { return scope_node->ctrl(n); }

Node *Parser::parseIntegerLiteral() {
  return (new ConstantNode(lexer->parseNumber(), START))->peephole();
}

Type *Lexer::parseNumber() {
  std::string snum = parseNumberString();
  if (snum.length() > 1 && snum[0] == '0')
    throw std::runtime_error(
        "Syntax error: integer values cannot start with '0'");
  return TypeInteger::constant(std::stol(snum));
}

bool Lexer::peek(char ch) {
  skipWhiteSpace();
  return peek() == ch;
}

bool Parser::peek(char ch) { return lexer->peek(ch); }
Node *Parser::parseExpression() { return parseComparison(); }

Node *Parser::parseComparison() {
  auto lhs = parseAddition(); // Parse the left-hand side

  if (match("==")) {
    return (new EQ(lhs, parseComparison()))->peephole();
  }

  if (match("!=")) {
    return (new NotNode((new EQ(lhs, parseComparison()))->peephole()))
        ->peephole();
  }

  if (match("<=")) {
    return (new LE(lhs, parseComparison()))->peephole();
  }

  if (match("<")) {
    return (new LT(lhs, parseComparison()))->peephole();
  }

  if (match(">=")) {
    return (new LE(parseComparison(), lhs))->peephole();
  }

  if (match(">")) {
    return (new LT(parseComparison(), lhs))->peephole();
  }

  return lhs;
}

Node *Parser::parseAddition() {
  Node *lhs = parseMultiplication();
  if (match("+"))
    return (new AddNode(lhs, parseAddition()))->peephole();
  if (match("-"))
    return (new SubNode(lhs, parseAddition()))->peephole();
  return lhs;
}

Node *Parser::parseMultiplication() {
  Node *lhs = parseUnary();
  if (match("*"))
    return (new MulNode(lhs, parseMultiplication()))->peephole();
  if (match("/"))
    return (new DivNode(lhs, parseMultiplication()))->peephole();
  return lhs;
}

Node *Parser::parseUnary() {
  if (match("-"))
    return (new MinusNode(parseUnary()))->peephole();
  return parsePrimary();
}

Node *Parser::parsePrimary() {
  lexer->skipWhiteSpace();
  if (lexer->isNumber())
    return parseIntegerLiteral();
  if (match("("))
    return require(parseExpression(), ")");
  if (matchx("true"))
    return (new ConstantNode(TypeInteger::constant(1), START))->peephole();
  if (matchx("true"))
    return (new ConstantNode(TypeInteger::constant(0), START))->peephole();
  std::string name = lexer->matchId();
  if (name == "")
    errorSyntax("an identifier or expression");
  Node *n = scope_node->lookup(name);
  if (n != nullptr)
    return n;
  throw std::runtime_error("Undefined name: '" + name + "'");
}

void Parser::require(std::string syntax) { require(nullptr, syntax); }

Node *Parser::parseDecl() {
  std::string name = requireId();
  require("=");
  auto expr = require(parseExpression(), ";");
  if (scope_node->define(name, expr) == nullptr)
    error("Redefining name '" + name + "'");
  return expr;
}

std::string Parser::requireId() {
  std::string id = lexer->matchId();
  if (id != "" && (KEYWORDS.find(id) == KEYWORDS.end()))
    return id;
  error("Expected an identifier, found " + id);
}

Node *Parser::require(Node *n, std::string syntax) {
  if (match(syntax))
    return n;
  errorSyntax(syntax);
}

void Parser::errorSyntax(std::string syntax) {
  error("Syntax error, expected " + syntax + ": " + lexer->getAnyNextToken());
}

void Parser::error(std::string errorMessage) {
  throw std::runtime_error(errorMessage);
}

bool Parser::match(std::string syntax) { return lexer->match(syntax); }

bool Parser::matchx(std::string syntax) { return lexer->matchx(syntax); }
void error(std::string errorMessage) { throw std::runtime_error(errorMessage); }

// Lexer
Lexer::Lexer(std::string &source) : input(source) {}

bool Lexer::isEof() { return position >= input.length(); }

char Lexer::peek() { return isEof() ? '\0' : input[position]; }

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
    if (input[position + i] != syntax[i]) {
      return false;
    }
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

std::string Lexer::get_input() { return input; }

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

std::string Lexer::matchId() {
  skipWhiteSpace();
  return isIdStart(peek()) ? parseId() : "";
}

std::string Lexer::parseNumberString() {
  std::size_t start = position;
  while (isNumber(nextChar()))
    ;
  return std::string(input, start, --position - start);
}

bool Lexer::isWhiteSpace() { return peek() <= ' ' && peek() != '\0'; }

void Lexer::skipWhiteSpace() {
  while (isWhiteSpace())
    position++;
}
