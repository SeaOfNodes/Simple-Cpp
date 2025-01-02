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
#include "../Include/node/xctrl_node.h"

#include "../Include/type/type_struct.h"

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

    // Return +len that ends a long
    // Return -len that ends a double
    int isLongOrDouble();
    
    std::size_t position = 0;
    bool peekIsId();

    char peek();
private:
    std::string input;


    bool isIdStart(char ch);

    bool isIdLetter(char ch);


    char nextChar();

    bool isWhiteSpace();

    std::string parsePunctuation();
};

class Parser {
public:
    ScopeNode *scope_node;
    static StartNode *START;
    static bool SCHEDULED;

    static StopNode *STOP;
    static ConstantNode* ZERO; // Very common node, cached here
    static XCtrlNode* XCTRL; // Very common node, cached here

    // Next available memory alias number
    static int ALIAS;
    Tomi::Vector<ScopeNode *> xScopes;

    ScopeNode *continueScope;
    ScopeNode *breakScope;

    static Node* con(long con);

    static Tomi::HashMap<std::string, Type *> TYPES;

    explicit Parser(std::string source, TypeInteger *arg);

    explicit Parser(std::string source);

    ~Parser();

    StopNode *parse();

    Type* type();
    TypeMemPtr* typeAry(Type* t);

    StopNode *parse(bool show);

    // zero/sign extend.  "i" is limited to either classic unsigned (min==0) or
    // classic signed (min=minus-power-of-2); max=power-of-2-minus-1.
    Node* ZSMask(Node* val, Type* t);
    std::string src();

    Node *ctrl();

    Node *ctrl(Node *n);

    bool peek(char ch);
    bool peekIsId();

    static std::string memName(int alias);

    Node* newStruct(TypeStruct* obj, Node* size);
    Node* newArray(TypeStruct* ary, Node* len);

    Node* memAlias(int alias);
    Node* memAlias(int alias, Node* st);

private:
    /**
     LIst of keywords disallowed as identifiers
     */

    // replace this with custom data structure
    const std::unordered_set <std::string> KEYWORDS = {
            "bool", "break", "byte", "continue", "else", "f32", "f64", "i16", "i32", "i64", "i8", "false", "if", "int", "return", "true", "u1", "u16", "u32", "u8", "while", "null", "new", "struct", "flt"};

    Node *parseStatement();

    Node *parseReturn();

    Node *parseExpression();

    Node *parseIf();

    /**
   * Parse a struct declaration, and return the following statement.
   * Only allowed in top level scope.
   * Structs cannot be redefined.
   *
   * @return The statement following the struct
   */
    Node *parseStruct();

    Field *parseField();

    Node *parseWhile();

    void checkLoopActive();

    ScopeNode *jumpTo(ScopeNode *toScope);

    Node *parseBreak();

    Node* parseBitWise();
    Node* parseShift();

    Node *parseContinue();

    Node *parseAddition();

    Node *parseMultiplication();

    Node *parseUnary();

    Node *parseComparison();

    Node *parsePrimary();

    Node* parsePostFix(Node* expr);

    Node *parseLiteral();

    Node *showGraph();

    Node *parseBlock();

    // require an exact match

    Parser* require(std::string syntax);

    template<typename N>
    N require(N n, std::string syntax) {
        if (match(syntax))
            return n;
        errorSyntax(syntax);
    };

    // Require and return an identifier
    std::string requireId();

    Node *parseExpressionStatement();

    void errorSyntax(std::string syntax);

    void error(std::string errorMessage);

    bool match(std::string syntax);

    bool matchx(std::string syntax);

    Lexer *lexer;
};

#endif
