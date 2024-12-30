#include "../Include/parser.h"
#include "../Include/graph_visualizer.h"
#include "../Include/node/new_node.h"
#include "../Include/node/store_node.h"
#include "../Include/node/load_node.h"
#include "../Include/node/cproj_node.h"
#include "../Include/type/type_float.h"
#include "../Include/node/to_float_node.h"

// bitiwse
#include "../Include/node/and_node.h"
#include "../Include/node/or_node.h"
#include "../Include/node/xor_node.h"
#include "../Include/node/shl_node.h"
#include "../Include/node/shr_node.h"
#include "../Include/node/sar_node.h"
#include "../Include/node/roundf32_node.h"

#include <cmath> // For std::abs
#include <bit>

// Todo: static fiasco?
StopNode *Parser::STOP = nullptr;
StartNode *Parser::START = nullptr;
ConstantNode *Parser::ZERO = nullptr;
XCtrlNode *Parser::XCTRL = nullptr;

Tomi::HashMap<std::string, Type *> Parser::TYPES = {};

Parser::Parser(std::string source, TypeInteger *arg) {
    Node::reset();
    IterPeeps::reset();

    TYPES.clear();

    TYPES.put("int", TypeInteger::BOT());
    TYPES.put("flt", TypeFloat::BOT());
    TYPES.put("bool", TypeInteger::U1());
    TYPES.put("byte", TypeInteger::U8());
    TYPES.put("f32", TypeFloat::B32());
    TYPES.put("f64", TypeFloat::BOT());
    TYPES.put("i16", TypeInteger::I16());
    TYPES.put("i32", TypeInteger::I32());
    TYPES.put("i64", TypeInteger::BOT());
    TYPES.put("i8", TypeInteger::I8());
    TYPES.put("u1", TypeInteger::U1());
    TYPES.put("u16", TypeInteger::U16());
    TYPES.put("u32", TypeInteger::U32());
    TYPES.put("u8", TypeInteger::U8());

    SCHEDULED = false;
    lexer = alloc.new_object<Lexer>(source);
    scope_node = alloc.new_object<ScopeNode>();
    continueScope = nullptr;
    breakScope = nullptr;

    START = alloc.new_object<StartNode>(std::initializer_list < Type * > {Type::CONTROL(), arg});
    STOP = alloc.new_object<StopNode>(std::initializer_list < Node * > {});
    ZERO = (dynamic_cast<ConstantNode *>(alloc.new_object<ConstantNode>(TypeInteger::constant(0),
                                                                        Parser::START)->peephole()->keep()));
    XCTRL = dynamic_cast<XCtrlNode *>((alloc.new_object<XCtrlNode>())->peephole()->keep());
    START->peephole();
}

bool Parser::SCHEDULED = false;

Parser::Parser(std::string source) : Parser(source, TypeInteger::BOT()) {}

Parser::~Parser() {
    delete lexer;
    delete scope_node;
    delete START;
    delete STOP;
}

StopNode *Parser::parse() { return parse(true); }

StopNode *Parser::parse(bool show) {
    xScopes.push_back(scope_node);
    scope_node->push();
    scope_node->define(ScopeNode::CTRL, Type::CONTROL(),
                       (alloc.new_object<CProjNode>(START, 0, ScopeNode::CTRL))->peephole());
    scope_node->define(ScopeNode::ARG0, TypeInteger::BOT(),
                       (alloc.new_object<ProjNode>(START, 1, ScopeNode::ARG0))->peephole());
    parseBlock();
    if (ctrl()->type_ == Type::CONTROL()) {
        STOP->addReturn((alloc.new_object<ReturnNode>(ctrl(), (alloc.new_object<ConstantNode>(TypeInteger::constant(0),
                                                                                              Parser::START))->peephole(),
                                                      scope_node))->peephole());

    }
    // before pop
    for (const auto &pair: scope_node->scopes[1]) {
        std::string name = pair.key;
    }
    scope_node->pop();
    for (const auto &pair: scope_node->scopes[1]) {
        std::string name = pair.key;
    }
    xScopes.pop_back();

    if (!lexer->isEof())
        throw std::runtime_error("Syntax error, unexpected " +
                                 lexer->getAnyNextToken());
    // type is not set for the second one here, for some reasons it disappears
    STOP = (StopNode *) STOP->peephole();
    if (show)
        showGraph();

    return STOP;
}

std::string Parser::src() { return lexer->get_input(); }

void Parser::checkLoopActive() {
    if (breakScope == nullptr)
        throw std::runtime_error("No active loop for a break or continue");
}

Node *Parser::parseBreak() {
    checkLoopActive();
    breakScope = dynamic_cast<ScopeNode *>(require(jumpTo(breakScope), ";"));
    return breakScope;
}

ScopeNode *Parser::jumpTo(ScopeNode *toScope) {
    ScopeNode *cur = scope_node->dup();
    ctrl(XCTRL);
//  ctrl((alloc.new_object<ConstantNode>(Type::XCONTROL(), Parser::START))
//           ->peephole()); // Kill current scope
    // Prune nested lexical scopes that have depth > than the loop head.
    while (cur->scopes.size() > breakScope->scopes.size()) {
        cur->pop();
    }
    // If this is a continue then first time the target is null
    // So we just use the pruned current scope as the base for the
    // continue
    if (toScope == nullptr) {
        std::ostringstream builder;
        return cur;
    }
    // toScope is either the break scope, or a scope that was created here
    assert(toScope->scopes.size() <= breakScope->scopes.size());
    std::ostringstream builder;
    toScope->ctrl(toScope->mergeScopes(cur));
    return toScope;
}

Node *Parser::parseContinue() {
    checkLoopActive();
    continueScope =
            dynamic_cast<ScopeNode *>(require(jumpTo(continueScope), ";"));
    return continueScope;
}

Node *Parser::parseStruct() {
    if (xScopes.size() > 1) throw std::runtime_error("struct declarations can only appear in top level scope");
    std::string typeName = requireId();
    Type** t = TYPES.get(typeName);
    if(t != nullptr && !(dynamic_cast<TypeStruct*>(*t))) throw std::runtime_error("struct " + typeName + " cannot be redefiend");

    Tomi::Vector<Field *> fields;
    require("{");
    while (!peek('}') && !lexer->isEof()) {
        Field *field = parseField();
        auto *it = std::find(fields.begin(), fields.end(), field);
        if (it != fields.end())
            throw std::runtime_error("Field '" + field->fname_ + "' already defined in struct '" + typeName + "'");
        fields.push_back(field);
    }
    require("}");
    // Build and install the TypeStruct
    TypeStruct *type = TypeStruct::make(typeName, fields);
    TYPES.put(typeName, type);
    START->addMemProj(type, scope_node);
    return parseStatement();
}

Field *Parser::parseField() {
    Type *t = type();
    if (t == nullptr) {
        throw std::runtime_error("A field type is expected, only type 'int' is supported at present");
    }
    return require(Field::make(requireId(), t), ";");

}

Node *Parser::parseStatement() {
    if (matchx("return"))
        return parseReturn();
    else if (match("{"))
        return require(parseBlock(), "}");
    else if (matchx("if"))
        return parseIf();
    else if (matchx("while"))
        return parseWhile();
    else if (matchx("break"))
        return parseBreak();
    else if (matchx("continue"))
        return parseContinue();
    else if (matchx("struct"))
        return parseStruct();
    else if (matchx("#showGraph"))
        return require(showGraph(), ";");
    else if (match(";"))
        return nullptr;
    else
        // declarations of vars with struct type are handled in parseExpressionStatement due
        // to ambiguity
        return parseExpressionStatement();
}

Node *Parser::parseWhile() {
    auto *savedContinueScope = continueScope;
    auto *savedBreakScope = breakScope;

    require("(");
    // Loop region has two control inputs, the first is the entry
    // point, and second is back edge that is set after loop is parsed
    // (see end_loop() call below).  Note that the absence of back edge is
    // used as an indicator to switch off peepholes of the region and
    // associated phis; see {@code inProgress()}.
    ctrl(
            (alloc.new_object<LoopNode>(ctrl()))->peephole()); // Note we set back edge to null here

    // At loop head, we clone the current Scope (this includes all
    // names in every nesting level within the Scope).
    // We create phis eagerly for all the names we find, see dup().

    // Save the current scope as the loop head
    auto *head = dynamic_cast<ScopeNode *>(scope_node->keep());

    // Clone the head Scope to create a new Scope for the body.
    // Create phis eagerly as part of cloning
    xScopes.push_back(scope_node = scope_node->dup(
            true)); // The true argument triggers creating phis

    // Parse predicate
    auto pred = require(parseExpression(), ")");
    // IfNode takes current control and predicate
    auto *ifNode = (IfNode *) ((alloc.new_object<IfNode>(ctrl(), pred)))->peephole();
    // Setup projection nodes
    Node *ifT = (alloc.new_object<CProjNode>(ifNode->keep(), 0, "True"))->peephole();
    ifT->keep();
    Node *ifF =
            (alloc.new_object<CProjNode>(ifNode->unkeep(), 1, "False"))->peephole();

    // Clone the body Scope to create the exit Scope
    // which accounts for any side effects in the predicate
    // The exit Scope will be the final scope after the loop,
    // And its control input is the False branch of the loop predicate
    // Note that body Scope is still our current scope
    ctrl(ifF);
    xScopes.push_back(breakScope = scope_node->dup());
    // No continues yet

    continueScope = nullptr;

    // Parse the true side, which corresponds to loop body
    // Our current scope is the body Scope
    ctrl(ifT->unkeep()); // set ctrl token to ifTrue projection
    parseStatement();
    // Continue scope

    if (continueScope != nullptr) {
        continueScope = jumpTo(continueScope);
        scope_node->kill();
        scope_node = continueScope;
    }
    // The true branch loops back, so whatever is current control (_scope.ctrl)
    // gets added to head loop as input. endLoop() updates the head scope, and
    // goes through all the phis that were created earlier. For each phi, it sets
    // the second input to the corresponding input from the back edge. If the phi
    // is redundant, it is replaced by its sole input.
    auto exit = breakScope;
    head->endLoop(scope_node, exit);
    // This assert fails
    head->unkeep()->kill();

    xScopes.pop_back(); // Cleanup
    xScopes.pop_back(); // Cleanup

    continueScope = savedContinueScope;
    breakScope = savedBreakScope;

    // At exit the false control is the current control, and
    // the scope is the exit scope after the exit test.
    return (scope_node = exit);
}

Node *Parser::parseIf() {
    require("(");
    // Parse predicate
    Node *pred = require(parseExpression(), ")")->keep();
    // IfNode takes current control and predicate
    auto *ifNode = ((alloc.new_object<IfNode>(ctrl(), pred)))->peephole();
    // Setup projection nodes
    Node *ifT = (alloc.new_object<CProjNode>(ifNode->keep(), 0, "True"))->peephole();
    // should be the if statement itself
    ifT->keep();

    Node *ifF =
            (alloc.new_object<CProjNode>(ifNode->unkeep(), 1, "False"))->peephole();
    // In if true branch, the ifT proj node becomes the ctrl
    // But first clone the scope and set it as current
    ifF->keep();

    std::size_t ndefs = scope_node->nIns();
    ScopeNode *fScope = scope_node->dup(); // Duplicate current scope
    xScopes.push_back(fScope); // For graph visualisation we need all scopes

    // Parse the true side
    ctrl(ifT->unkeep()); // set ctrl token to ifTrue projection
    scope_node->upcast(ifT, pred, false); // up-cast predicate
    parseStatement();    // Parse true-side
    ScopeNode *tScope = scope_node;

    scope_node = fScope;
    ctrl(ifF->unkeep());
    scope_node->upcast(ifF, pred, true); // Up-cast predicate
    if (matchx("else")) {
        parseStatement();
        fScope = scope_node;
    }
    pred->unkeep();

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

Type *Parser::type() {
    size_t old = lexer->position;
    std::string tname = lexer->matchId();
    if (tname.empty()) return nullptr;
    bool nullable = match("?");
    Type **t = TYPES.get(tname);
    // Assume a forward-reference type
    Type*a;
    if (t == nullptr) {
        std::size_t old2 = lexer->position;
        std::string id = lexer->matchId();
        if(id.empty()) {
            lexer->position = old;
            return nullptr;
        }
        a = TypeStruct::make(tname);
        // Not a type; unwind the parse
        TYPES.put(tname, a);
        lexer->position = old2;
    }
    auto*c = t ? *t : a;
    auto*obj = dynamic_cast<TypeStruct*>(c);
    return obj ? TypeMemPtr::make(obj, nullable) : c;
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
    size_t old = lexer->position;
    Type *t = type();
    std::string name = requireId();
    Node *expr;

    if (match(";")) {
        // No type and no expr is an error
        if (t == nullptr) error("Expected a type or expression");
        expr = (alloc.new_object<ConstantNode>(t->makeInit(), Parser::START))->peephole();
    } else if (match("=")) {
        // Assign "= expr;"
        if(name == "i") {
            std::cerr << "Here";
        }
        expr = require(parseExpression(), ";");
    } else {
        lexer->position = old;
        return require(parseExpression(), ";");
    }


    // Defining a new variable vs updating an old one
    if (t != nullptr) {
        if (scope_node->define(name, t, expr) == nullptr) error("Redefining name `" + name + "`");
    } else {
        Node *n = scope_node->lookup(name);
        if (n == nullptr) error("Undefined name: '" + name + "'");
        t = scope_node->lookUpDeclaredType(name);
    }

    // Auto-widen int to float
    if (dynamic_cast<TypeInteger *>(expr->type_) && dynamic_cast<TypeFloat *>(t)) {
        expr = (alloc.new_object<ToFloatNode>(expr))->peephole();
    }

    // Auto-narrow wide ints to narrow ints
    expr = ZSMask(expr, t);
    // Auto-deepen forward ref types
    Type* e = expr->type_;
    auto* tmp = dynamic_cast<TypeMemPtr*>(e);
    if(tmp && !tmp->obj_->fields_) {
        e = tmp->make_from(dynamic_cast<TypeStruct*>(*TYPES.get(tmp->obj_->name_)));
    }
    if(!e->isa(t)) {
        std::cerr << "Here";
    }
    if (!e->isa(t)) error("Type " + e->str() + " is not of declared type " + t->str());
    return scope_node->update(name, expr);

}

Node *Parser::parseReturn() {
    Node *expr = require(parseExpression(), ";");
    Node *bpeep = (alloc.new_object<ReturnNode>(ctrl(), expr, scope_node))->peephole();
    auto *ret = STOP->addReturn(bpeep);
    ctrl(XCTRL); // kill control
    return ret;
}

Node *Parser::ctrl() { return scope_node->ctrl(); }

Node *Parser::ctrl(Node *n) {
    return scope_node->ctrl(n);
}

Node *Parser::parseLiteral() {
    Type*t = lexer->parseNumber();
    return (alloc.new_object<ConstantNode>(t, START))->peephole();
}

Node* Parser::parseBitWise() {
    Node*lhs = parseComparison();
    while(true) {
        if(false);
        else if(match("&")) {
            lhs = (alloc.new_object<AndNode>(lhs, nullptr));
        } else if(match("|")) {
            lhs = (alloc.new_object<OrNode>(lhs, nullptr));
        } else if(match("^")) {
            lhs = (alloc.new_object<XorNode>(lhs, nullptr));
        } else break;
        if(dynamic_cast<XorNode*>(lhs)) {
            std::cerr << "Here";
        }
        lhs->setDef(2, parseComparison());
        lhs = lhs->peephole();
        std::cerr << "Here";
    }
    return lhs;
}
Node* Parser::parseShift() {
    Node*lhs = parseAddition();
    while(true) {
        if(false);
        else if(match("<<")) {
            lhs = (alloc.new_object<ShlNode>(lhs, nullptr));
        } else if(match(">>>")) {
            lhs = (alloc.new_object<ShrNode>(lhs, nullptr));
        } else if(match(">>")) {
            lhs = (alloc.new_object<SarNode>(lhs, nullptr));
        } else break;
        lhs->setDef(2, parseAddition());
        lhs = lhs->peephole();
    }
    return lhs;
}
Type *Lexer::parseNumber() {
    int old = position;
    int len = isLongOrDouble();
    if (len > 0) {
        if (len > 1 && input[old] == '0') {
            throw std::runtime_error(
                    "Syntax error: integer values cannot start with '0'");
        }
        long a = std::stol(input.substr(old, len));
        return dynamic_cast<Type *>(TypeInteger::constant(a));
    }
    // TBD;
    return TypeFloat::constant(std::stod(input.substr(old, -len)));
}

bool Lexer::peek(char ch) {
    skipWhiteSpace();
    return peek() == ch;
}

bool Parser::peek(char ch) { return lexer->peek(ch); }

Node *Parser::parseExpression() { return parseBitWise(); }

Node *Parser::parseComparison() {
    auto lhs = parseShift(); // Parse the left-hand side
    while (true) {
        int idx = 0;
        bool negate = false;
        if (false);
        else if (match("==")) {
            idx = 2;
            lhs = alloc.new_object<EQ>(lhs, nullptr);
        } else if (match("!=")) {
            idx = 2;
            lhs = alloc.new_object<EQ>(lhs, nullptr);
            negate = true;
        } else if (match("<=")) {
            idx = 2;
            lhs = alloc.new_object<LE>(lhs, nullptr);
        } else if (match("<")) {
            idx = 2;
            lhs = alloc.new_object<LT>(lhs, nullptr);
        } else if (match(">=")) {
            idx = 1;
            lhs = alloc.new_object<LE>(nullptr, lhs);
        } else if (match(">")) {
            idx = 1;
            lhs = alloc.new_object<LT>(nullptr, lhs);
        } else
            break;
        lhs->setDef(idx, parseAddition());
        lhs = lhs->widen()->peephole();
        if (negate) {
            lhs = (alloc.new_object<NotNode>(lhs))->peephole();
        }
    }
    return lhs;
}

Node *Parser::parseAddition() {
    Node *lhs = parseMultiplication();
    while (true) {
        if (false);
        else if (match("+")) {
            lhs = alloc.new_object<AddNode>(lhs, nullptr);
        } else if (match("-")) {
            lhs = alloc.new_object<SubNode>(lhs, nullptr);
        } else
            break;
        lhs->setDef(2, parseMultiplication());
        if(dynamic_cast<AddNode*>(lhs->in(1))) {
            std::cerr << "Here";
        }
        lhs = lhs->widen()->peephole(); // new id because new replacement WRONG!!
        std::cerr << "Here";
    }
    return lhs;
}

Node *Parser::parseMultiplication() {
    Node *lhs = parseUnary();
    while (true) {
        if (false);
        else if (match("*")) {
            lhs = alloc.new_object<MulNode>(lhs, nullptr);
        } else if (match("/")) {
            lhs = alloc.new_object<DivNode>(lhs, nullptr);
        } else
            break;
        lhs->setDef(2, parseUnary());
        lhs = lhs->widen()->peephole();
    }
    return lhs;
}

Node *Parser::parseUnary() {
    if (match("-"))
        return (alloc.new_object<MinusNode>(parseUnary()))->widen()->peephole();
    if (match("!")) return (alloc.new_object<NotNode>(parseUnary()))->peephole();
    return parsePostFix(parsePrimary());
}

Node *Parser::memAlias(int alias) {
    return scope_node->lookup(memName(alias));
}

Node *Parser::memAlias(int alias, Node *st) {
    return scope_node->update(memName(alias), st);
}

Node *Parser::newStruct(TypeStruct *obj) {
    Node *n = (alloc.new_object<NewNode>(TypeMemPtr::make(obj), ctrl()))->peephole();
    int *alias = StartNode::aliasStarts.get(obj->name_);
    assert(alias != nullptr);

    for (Field *field: obj->fields_.value()) {
        //             memAlias(alias, new StoreNode(field._fname, alias, memAlias(alias), n, initValue).peephole());


        memAlias(*alias,
                 (alloc.new_object<StoreNode>(field->fname_, *alias, field->type_, ctrl(),
                 memAlias(*alias), n, (alloc.new_object<ConstantNode>(field->type_->makeInit(), Parser::START))->peephole(),
                 true))->peephole());

        alias++;
    }
    return n->unkeep();
}

Node* Parser::ZSMask(Node *val, Type *t) {
    auto*tval = dynamic_cast<TypeInteger*>(val->type_);
    auto*t0 = dynamic_cast<TypeInteger*>(t);
    auto*tval1 =dynamic_cast<TypeFloat*>(val->type_);
    auto*t1 = dynamic_cast<TypeFloat*>(t);
    if(!(tval && t0 && !tval->isa(t0))) {
        if(!(tval1 && t1 && !tval1->isa(t1))) {
            return val;
        }
        // Float rounding
        return (alloc.new_object<RoundF32Node>(val))->peephole();
}
    if(t0 && t0->min_ == 0)  // Unsigned
    {
        return alloc.new_object<AndNode>(val, alloc.new_object<ConstantNode>(TypeInteger::constant(t0->max_),
                                                                              Parser::START)->peephole())->peephole();
    }
    // Signed extension
    int shift = std::countl_zero(static_cast<std::uint64_t>(t0->max_)) - 1;
    Node*shf = alloc.new_object<ConstantNode>(TypeInteger::constant(shift), Parser::START)->peephole();

    if(shf->type_ == TypeInteger::ZERO()) return val;
    return (alloc.new_object<SarNode>(alloc.new_object<ShlNode>(val, shf->keep())->peephole(), shf->unkeep())->peephole());
}

Node *Parser::parsePostFix(Node *expr) {
    bool match_s = match(".");
    if (!match_s) return expr;
    auto *ptr = dynamic_cast<TypeMemPtr *>(expr->type_);

    if(expr->nid == 30) {
        std::cout << "here";
    }
    if (!ptr) {
        error("Expected struct reference but got " + expr->type_->str());
    }
    std::string name = requireId();
    if (expr->type_ == TypeMemPtr::TOP()) throw std::runtime_error("Accessing field '" + name + "'from nullptr");
    if(ptr->obj_ == nullptr) throw std::runtime_error("Accessing unknown field '" + name + "' from '" + ptr->str() + "'");
    auto* base = dynamic_cast<TypeStruct*>(*TYPES.get(ptr->obj_->name_));
    int idx = base == nullptr ? -1 : base->find(name);

    if (idx == -1) error("Accessing unknown field '" + name + "' from '" + ptr->str() + "'");
    int alias = *(StartNode::aliasStarts.get(ptr->obj_->name_)) + idx;
    if (match("=")) {
        // Disambiguate "obj.fld==x" boolean test from "obj.fld=x" field assignment
        if (peek('=')) lexer->position--;
        else {
            Node *val = parseExpression();
            Type*glb = base->fields_.value()[idx]->type_;
            // auto truncate when storing to narrow fields
            val = ZSMask(val, glb);
            memAlias(alias, (alloc.new_object<StoreNode>(name, alias, glb, ctrl(), memAlias(alias), expr, val, false)))->peephole();
            return expr;    // "obj.a = expr" returns the expression while updating memory

        }
    }
    Type *declaredType = base->fields_.value()[idx]->type_;
    return parsePostFix((alloc.new_object<LoadNode>(name, alias, declaredType->glb(), memAlias(alias), expr))->peephole());
}

Node *Parser::parsePrimary() {
    lexer->skipWhiteSpace();
    if (lexer->isNumber(lexer->peek()))
        return parseLiteral();
    if (match("("))
        return require(parseExpression(), ")");
    if (matchx("true"))
        return (alloc.new_object<ConstantNode>(TypeInteger::constant(1), START))->peephole();
    if (matchx("false"))
        return ZERO;
    if (matchx("null")) return (alloc.new_object<ConstantNode>(TypeMemPtr::NULLPTR(), Parser::START))->peephole();
    if (matchx("new")) {
        std::string structName = requireId();
        Type **t = TYPES.get(structName);
        auto* obj = dynamic_cast<TypeStruct *>(*t);
        if (!(obj) || !obj->fields_) error("Unknown struct type: " + structName);
        return newStruct(obj);
    }
    std::string name = lexer->matchId();
    if (name == "")
        errorSyntax("an identifier or expression");
    Node *n = scope_node->lookup(name);
    if(name == "i") {
        std::cerr << "Here";
    }
    std::ostringstream b;
    std::string arg_type = n->type_->print_1(b).str();
    if (n != nullptr)
        return n;
    throw std::runtime_error("Undefined name: '" + name + "'");
}

bool Parser::peekIsId() {

    return lexer->peekIsId();
}

bool Lexer::peekIsId() {
    skipWhiteSpace();
    return isIdStart(peek());
}

Parser *Parser::require(std::string syntax) {
    require(nullptr, syntax);
    return this;
}

std::string Parser::memName(int alias) {
    return "$" + std::to_string(alias);
}

std::string Parser::requireId() {
    std::string id = lexer->matchId();
    if (id != "" && (KEYWORDS.find(id) == KEYWORDS.end()))
        return id;

    if (id == "") {
        error("Expected an identifier, found 'NULLPTR'");
    }
    error("Expected an identifier, found " + id);
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
    while (isIdLetter(nextChar()));
    return std::string(input, start, --position - start);
}

std::string Lexer::parsePunctuation() {
    std::size_t start = position;
    return std::string(input, start, 1);
}

std::string Lexer::matchId() {
    return peekIsId() ? parseId() : "";
}

std::string Lexer::parseNumberString() {
    int old = position;
    int len = std::abs(isLongOrDouble());
    position += len;

    return std::string(input, old, len);
}

bool Lexer::isWhiteSpace() { return peek() <= ' ' && peek() != '\0'; }

int Lexer::isLongOrDouble() {
    int old = position;
    char c;
    while (isdigit(c = nextChar())) { }
    if (!(c == 'e' || c == '.')) {
        return --position - old;
    }
    while (isdigit(c = nextChar()) || c == 'e' || c == '.') { }
    return -(--position - old);
}

void Lexer::skipWhiteSpace() {
    while (true) {
        if (isWhiteSpace()) position++;
            // Skip // to the end of line
        else if (position + 2 < input.length() &&
                 input[position] == '/' &&
                 input[position + 1] == '/') {
            position += 2;
            while (!isEof() && input[position] != '\n') position++;
        } else break;
    }
    while (isWhiteSpace())
        position++;
}
