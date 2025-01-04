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

#include "../Include/node/scope_minnode.h"
#include "../Include/node/struct_node.h"

#include <cmath> // For std::abs
#include <bit>

// Todo: static fiasco?
StopNode *Parser::STOP = nullptr;
StartNode *Parser::START = nullptr;
ConstantNode *Parser::ZERO = nullptr;
XCtrlNode *Parser::XCTRL = nullptr;

Tomi::Vector<Node*> Parser::ALIMP = {};
int Parser::ALIAS = 2;

Tomi::HashMap<std::string, Type *> Parser::TYPES = {};
Tomi::HashMap<std::string, StructNode*> Parser::INITS = {};

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

    START = alloc.new_object<StartNode>(arg);
    STOP = alloc.new_object<StopNode>(std::initializer_list < Node * > {});
    ZERO = dynamic_cast<ConstantNode *>(con(static_cast<long>(0))->keep());
    XCTRL = dynamic_cast<XCtrlNode *>((alloc.new_object<XCtrlNode>())->peephole()->keep());
}

bool Parser::SCHEDULED = false;

Parser::Parser(std::string source) : Parser(source, TypeInteger::BOT()) {}

Node *Parser::con(long con) {
    return (alloc.new_object<ConstantNode>(TypeInteger::constant(con), Parser::START))->peephole();
}

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
    ScopeMinNode *mem = alloc.new_object<ScopeMinNode>();
    mem->addDef(nullptr);

    scope_node->define(ScopeNode::CTRL, Type::CONTROL(),  false,
                       (alloc.new_object<CProjNode>(START, 0, ScopeNode::CTRL))->peephole());

    mem->addDef(alloc.new_object<ProjNode>(START, 1, ScopeNode::MEM0)->peephole());
    scope_node->define(ScopeNode::MEM0, TypeMem::TOP(), false, mem->peephole());

    scope_node->define(ScopeNode::ARG0, TypeInteger::BOT(), false,
            (alloc.new_object<ProjNode>(START, 2, ScopeNode::ARG0))->peephole());
    parseBlock();
    if (ctrl()->type_ == Type::CONTROL()) {
        STOP->addReturn((alloc.new_object<ReturnNode>(ctrl(), ZERO,
                                                      scope_node))->peephole());

    }
    // before pop
    scope_node->pop();

    xScopes.pop_back();

    for (auto a:  Parser::INITS) {
        a.val->unkeep()->kill();
    }
    INITS.clear();
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
    while (cur->lexSize.size() > breakScope->lexSize.size()) {
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
    assert(toScope->lexSize.size() <= breakScope->lexSize.size());
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
    Type **t = TYPES.get(typeName);
    if (t != nullptr) {
        auto *tmp = dynamic_cast<TypeMemPtr *>(*t);
        if (!(tmp && !tmp->obj_->fields_.has_value()))
            throw std::runtime_error("struct " + typeName + " cannot be redefined");
    }

    // Parse a collection of fields
    Tomi::Vector<Field *> fields;
    require("{");
    // A Block scope parse, and inspect the scope afterward for fields.
    scope_node->push();
    while (!peek('}') && !lexer->isEof()) {
        parseStatement();
    }
    // Grab the declarations and build fields and a Struct
    int lexlen = scope_node->lexSize.size();
    int varlen = scope_node->vars.size();
    StructNode* s = new StructNode;
    Tomi::Vector<Field*> fs(varlen - lexlen);
    for(int i = lexlen; i < varlen; i++) {
        s->addDef(scope_node->in(i));
        ScopeMinNode::Var* v=  scope_node->vars[i];
        fs[i-lexlen] = Field::make(v->name_, v->type_, ALIAS++, v->final_);
    }
    TypeStruct* ts = s->ts_ = TypeStruct::make(typeName, fs);
    TYPES.put(typeName, TypeMemPtr::make(ts));
    INITS.put(typeName, dynamic_cast<StructNode*>(s->peephole()->keep()));
    // Done with struct/block scope
    require("}");
    require(";");
    scope_node->pop();
    return nullptr;
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

Node *Parser::parseAsgn(Type *t, bool xfinal) {
    // Having a type is a declaration, missing one is updating a prior name
    bool isDecl = t != nullptr;
    int old = lexer->position;
    std::string name = requireId();

    Node *expr;
    if (peek(';') || peek(',')) {
        // Bare "name" is not allowed
        if (t == nullptr) throw std::runtime_error("expression");
        // Does this declaration need an initializer and not getting one?
        auto *tmp = dynamic_cast<TypeMemPtr *>(t);
        Type *init = xfinal || tmp && !tmp->nil_ ? Type::TOP() :
                                             // Else takes the default
                                             t->makeInit();

        expr = con(init);

    } else if (!match("=")) {  // Something else
        lexer->position = old;
        return nullptr;
    } else {
        expr = parseExpression();
    }
    // Defining a new variable vs updating an old one
    if (!isDecl) { // Assigning over an existing name
        // Lookup
        ScopeMinNode::Var* def = scope_node->lookup(name);
        if (def == nullptr) throw std::runtime_error("Undefined name '" + name + "'");
        // TOP fields are for late-initialized fields; these have never
        // been written to, and this must be the final write.  Other writes
        // need to check the final bit.
        if (scope_node->in(def->idx_)->type_ != Type::TOP() && def->final_) {
            error("Cannot reassign final '" + name + "'");

        }
        t = def->type_; // Declared field type
    }

    // Auto-widen int to float
    // Auto-widen int to float
    if (dynamic_cast<TypeInteger *>(expr->type_) && dynamic_cast<TypeFloat *>(t)) {
        expr = (alloc.new_object<ToFloatNode>(expr))->peephole();
    }

    // Auto-narrow wide ints to narrow ints
    expr = ZSMask(expr, t);
    // Auto-deepen forward ref types
    Type *e = expr->type_;
    auto *tmp = dynamic_cast<TypeMemPtr *>(e);
    if (tmp && !tmp->obj_->fields_) {
        e = *TYPES.get(tmp->obj_->name_);
    }
    if (!e->isa(t)) error("Type " + e->str() + " is not of declared type " + t->str());

    if (isDecl) {
        if (!scope_node->define(name, t, xfinal, expr)) throw std::runtime_error("Redefining name '" + name + "'");

    } else {
        scope_node->update(name, expr);
    }
    return expr;
}

Node *Parser::parseFinal(Type *t) {
    bool xfinal = t != nullptr && match("!");
    return parseAsgn(t, xfinal);
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
    xScopes.pop_back();
    xScopes.push_back(exit);
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
    Node* la = scope_node->in(2);

    ScopeNode *fScope = scope_node->dup(); // Duplicate current scope
    Node* fa = fScope->in(2);
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

    Node*b = fScope->in(2);
    Node *c = tScope->in(2);
    // Merge scope here
    return ctrl(tScope->mergeScopes(fScope));
}

Node *Parser::showGraph() {
    std::cout << GraphVisualizer().generateDotOutput(*this);
    return nullptr;
}

TypeMemPtr *Parser::typeAry(Type *t) {
    auto *tmp = dynamic_cast<TypeMemPtr *>(t);
    if (tmp && !tmp->nil_) throw std::runtime_error("Arrays of reference types must always be nullable");
    std::string tname = "[" + t->str() + "]";
    Type **ta = TYPES.get(tname);
    if (ta != nullptr) {
        Type *nptr = *ta;
        return dynamic_cast<TypeMemPtr *>(nptr);
    }
    // Need make an array type.
    // Separate increments
    int firstAlias = ALIAS++;
    int secondAlias = ALIAS++;

    TypeStruct *ts = TypeStruct::make_Ary(TypeInteger::BOT(), firstAlias, t, secondAlias);
    TypeMemPtr *tary = TypeMemPtr::make(ts);
    TYPES.put(tname, tary);
    START->addMemProj(ts, scope_node); // Insert memory alias edges
    return tary;
}

Type *Parser::type() {
    size_t old_1 = lexer->position;
    std::string tname = lexer->matchId();
    if (tname.empty()) return nullptr;
    // Convert the type name to a type.
    Type **t0 = TYPES.get(tname);
    Type *t1 = t0 == nullptr ? TypeMemPtr::make(TypeStruct::make(tname)) : *t0;
    // Nest arrays and '?' as needed

    Type *a;
    Type *nptr = t0 ? *t0 : nullptr;

    while (true) {
        if (match("?")) {
            auto *tmp = dynamic_cast<TypeMemPtr *>(t1);
            if (!tmp) {
                throw std::runtime_error("Type" + nptr->str() + "cannot be null");

            }
            if (tmp->nil_) throw std::runtime_error("Type " + t1->str() + "already allows null");
            t1 = TypeMemPtr::make(tmp->obj_, true);
            continue;
        }
        if (match("[]")) {
            t1 = typeAry(t1);
            continue;
        }
        break;
    }

    // Check no forward ref
    if (nptr != nullptr) {
        return t1;
    }
    // Check valid forward ref, after parsing all the type extra bits.
    // Cannot check earlier, because cannot find required 'id' until after "[]?" syntax
    int old_2 = lexer->position;
    std::string id = lexer->matchId();
    lexer->position = old_2;
    if (id.empty()) {
        lexer->position = old_1;
        return nullptr;
    }
    // Yes a forward ref, so declare it
    TYPES.put(tname, t1);
    return t1;
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
    Node *n;
    if (t != nullptr) {
        // now parse final [, final]*
        n = parseFinal(t);
        while (match(",")) {
            n = parseFinal(t);
        }
    } else if ((n = parseAsgn(nullptr, false)) == nullptr) {
        // Something else
        n = parseExpression();
    }
    return require(n, ";");

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
    Type *t = lexer->parseNumber();
    return con(t);
}

ConstantNode* Parser::con(Type*t) {
    return dynamic_cast<ConstantNode*>(alloc.new_object<ConstantNode>(t, Parser::START)->peephole());
}
Node *Parser::parseBitWise() {
    Node *lhs = parseComparison();
    while (true) {
        if (false);
        else if (match("&")) {
            lhs = (alloc.new_object<AndNode>(lhs, nullptr));
        } else if (match("|")) {
            lhs = (alloc.new_object<OrNode>(lhs, nullptr));
        } else if (match("^")) {
            lhs = (alloc.new_object<XorNode>(lhs, nullptr));
        } else break;
        lhs->setDef(2, parseComparison());
        lhs = lhs->peephole();
    }
    return lhs;
}

Node *Parser::parseShift() {
    Node *lhs = parseAddition();
    while (true) {
        if (false);
        else if (match("<<")) {
            lhs = (alloc.new_object<ShlNode>(lhs, nullptr));
        } else if (match(">>>")) {
            lhs = (alloc.new_object<ShrNode>(lhs, nullptr));
        } else if (match(">>")) {
            lhs = (alloc.new_object<SarNode>(lhs, nullptr));
        } else break;
        lhs->setDef(2, parseAddition());
        lhs = lhs->peephole();
    }
    return lhs;
}

Type *Lexer::parseNumber() {
    int old = static_cast<int>(position);
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
        lhs = lhs->widen()->peephole(); // new id because new replacement WRONG!!
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
    return scope_node->mem(alias);
}

void Parser::memAlias(int alias, Node *st) {
    scope_node->mem(alias, st);
}

Node *Parser::newStruct(TypeStruct *obj, Node *size, int idx, Tomi::Vector<Node*> init) {
    Tomi::Vector<Field *> fs = obj->fields_.value();
    if(fs.empty()) {
        error("Unknown struct type'" + obj->name_ + "'");
    }
    int len = fs.size();
    Tomi::Vector<Node *> ns(2 + len + len);

    ns[0] = ctrl();
    ns[1] = size;
    for (int i = 0; i < len; i++) {
        ns[i + 2] = memAlias(fs[i]->alias_);
    }
    // Initial values for every field
    for(int i = 0; i < len; i++) {
        ns[2 + len + i] = init[i + idx];
    }
    TypeMemPtr*m = TypeMemPtr::make(obj);

    Node *nnn = (alloc.new_object<NewNode>(TypeMemPtr::make(obj), ns));

    nnn = nnn->peephole();

    for (int i = 0; i < len; i++) {
        Node *pr = alloc.new_object<ProjNode>(nnn, i + 2, memName(fs[i]->alias_))->peephole();
        memAlias(fs[i]->alias_, pr);
    }
    Node *r = (alloc.new_object<ProjNode>(nnn, 1, obj->name_))->peephole();
    return r;
}

Node *Parser::ZSMask(Node *val, Type *t) {
    auto *tval = dynamic_cast<TypeInteger *>(val->type_);
    auto *t0 = dynamic_cast<TypeInteger *>(t);
    auto *tval1 = dynamic_cast<TypeFloat *>(val->type_);
    auto *t1 = dynamic_cast<TypeFloat *>(t);
    if (!(tval && t0 && !tval->isa(t0))) {
        if (!(tval1 && t1 && !tval1->isa(t1))) {
            return val;
        }
        // Float rounding
        return (alloc.new_object<RoundF32Node>(val))->peephole();
    }
    if (t0 && t0->min_ == 0)  // Unsigned
    {
        return alloc.new_object<AndNode>(val, con(t0->max_))->peephole();

    }
    // Signed extension
    int shift = std::countl_zero(static_cast<std::uint64_t>(t0->max_)) - 1;
    Node *shf = con(shift);

    if (shf->type_ == TypeInteger::ZERO()) return val;
    return (alloc.new_object<SarNode>(alloc.new_object<ShlNode>(val, shf->keep())->peephole(),
                                      shf->unkeep())->peephole());
}

bool Parser::matchOpx(char c0, char c1) {
    return lexer->matchOpx(c0, c1);

}
Node *Parser::parsePostFix(Node *expr) {
    std::string name;
    if (match(".")) name = requireId();
    else if (match("#")) name = "#";
    else if (match("[")) name = "[]";
    else return expr;

    // Sanity check expr for being a reference
    auto *ptr = dynamic_cast<TypeMemPtr *>(expr->type_);
    if (!ptr) {
        error("Expected a reference type, got " + expr->type_->str() + " instead");
    }

    // Happens when parsing known dead code, which often has other typing
    // issues.  Since the code is dead, possibly due to inlining, lets not
    // spoil the user experience with error messages.

    if(ctrl()->type_ == Type::XCONTROL()) {
        return matchOpx('=', '=') ? parseExpression() : parsePostFix(con(Type::TOP()));

    }
//
//    if (ptr == TypeMemPtr::TOP()) throw std::runtime_error("Accessing field '" + name + "'from nullptr");
//    if (ptr->obj_ == nullptr)
//        throw std::runtime_error("Accessing unknown field '" + name + "' from '" + ptr->str() + "'");
    auto *tmp = dynamic_cast<TypeMemPtr *>(*TYPES.get(ptr->obj_->name_));
    if (tmp == nullptr) {
        error("Accessing unknown struct type '" + ptr->obj_->name_ + "' from '" + ptr->str() + "'");
    }
    TypeStruct *base = tmp->obj_;
    int fidx = base->find(name);
    if (fidx == -1) {
        error("Accessing unknown field '" + name + "' from '" + ptr->str() + "'");
    }

    // Get field type and layout offset from base type and field index fidx
    Field *f = base->fields_.value()[fidx];
    Node *off;
    if (name == "[]") {    // If field is an array body
        // Array index math
        Node *idx = require(parseExpression(), "]");
        Node *shl = alloc.new_object<ShlNode>(idx, con(base->aryScale()))->peephole();
        int basel = base->aryBase();
        off = alloc.new_object<AddNode>(con(base->aryBase()), shl)->peephole();

    } else {
        // Hardwired field offset
        int val = base->offset(fidx);
        off = con(val);
    }
    if (match("=")) {
        if (peek('=')) lexer->position--;
        else {
            off->keep();
            Node *val = parseExpression();
            val = ZSMask(val, f->type_)->keep();
            Node *st = alloc.new_object<StoreNode>(name, f->alias_, f->type_, memAlias(f->alias_), expr->unkeep(),
                                                   off->unkeep(), val, false);

            // Arrays include control, as a proxy for a safety range check.
            // Structs don't need this; they only need a NPE check which is
            // done via the type system.
            if (base->isAry()) st->setDef(0, ctrl());
            memAlias(f->alias_, st->peephole());
            return val->unkeep();
        }
    }
    Node *load = (alloc.new_object<LoadNode>(name, f->alias_, f->type_->glb(), memAlias(f->alias_), expr,
                                             off));
    // Arrays include control, as a proxy for a safety range check
    // Structs don't need this; they only need a NPE check which is
    // done via the type system.
    if (base->isAry()) load->setDef(0, ctrl());
    return parsePostFix(load->peephole());
}

Node *Parser::newArray(TypeStruct *ary, Node *len) {
    int base = ary->aryBase();
    int scale = ary->aryScale();
    Node *size = alloc.new_object<AddNode>(con(base),
                                           alloc.new_object<ShlNode>(len, con(scale))->peephole())->peephole();

    ALIMP.clear();
    ALIMP.push_back(len);
    ALIMP.push_back(con(ary->fields_.value()[1]->type_->makeInit()));
    return newStruct(ary, size, 0, ALIMP);
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
    if (matchx("new")) return alloc_();
//        Type *t = type();
//        if (t == nullptr) error("Expected a type after 'new'");
//        if (match("[")) {
//            Node *len = parseExpression()->keep();
//            if (dynamic_cast<TypeInteger *>(len->type_) == nullptr) error("Array length must be an integer");
//            require("]");
//            TypeMemPtr *tmp = typeAry(t);
//            return newArray(tmp->obj_, len);
//        }
//        if (auto *tmp = dynamic_cast<TypeMemPtr *>(t)) {
//            TypeStruct *obj = tmp->obj_;
//            if (!obj->fields_.has_value()) {
//                error("Unknown struct type '" + obj->name_ + "'");
//            }
//            if (obj->isAry()) {
//                throw std::runtime_error("TODO");
//            }
//            return newStruct(obj, con(obj->offset(obj->fields_.value().size())));
//
//        }
////        std::string structName = requireId();
////        Type **t = TYPES.get(structName);
////        auto* obj = dynamic_cast<TypeStruct *>(*t);
////        if (!(obj) || !obj->fields_) error("Unknown struct type: " + structName);
////        return newStruct(obj);
//    }
    std::string name = lexer->matchId();
    if (name.empty()) throw std::runtime_error("an identifiero r expression");
    ScopeMinNode::Var* n = scope_node->lookup(name);
    if (n != nullptr) return scope_node->in(n);

//    Node *n = scope_node->lookup(name);
//
//    std::ostringstream b;
//    if (n != nullptr)
//        return n;
    throw std::runtime_error("Undefined name: '" + name + "'");
}

Node *Parser::alloc_() {
    Type *t = type();
    if (t == nullptr) error("Expected a type");
    // Parse ary[ length_expr ]
    if (match("[")) {
        Node *len = parseExpression()->keep();
        if (!dynamic_cast<TypeInteger *>(len->type_)) {
            error("Cannot allocate an array with length" + len->type_->str());

        }
        require("]");
        TypeMemPtr *tmp = typeAry(t);
        return newArray(tmp->obj_, len);
    }
    auto*tmp = dynamic_cast<TypeMemPtr*>(t);
    if (!tmp) throw std::runtime_error("Cannot allocate a " + t->str());
    // Parse new struct { default_initialization }
    StructNode **s = INITS.get(tmp->obj_->name_);
    if (s == nullptr) throw std::runtime_error("Unknown struct type: " + tmp->obj_->name_ + "'");
    StructNode* nptr = *s;
    Tomi::Vector<Field *> fs = nptr->ts_->fields_.value();
    // if the object is fully initialized, we can skip a block here.
    // Check for constructor block:
    bool hasConstdructor = match("{");
    Tomi::Vector<Node *> init = nptr->inputs;
    int idx = 0;
    if (hasConstdructor) {
        idx = scope_node->nIns();
        // Push a scope, and pre-assign all struct fields.
        scope_node->push();
        for (int i = 0; i < fs.size(); i++) {
            scope_node->define(fs[i]->fname_, fs[i]->type_, fs[i]->final_, nptr->in(i));
        }
        // Parse the constructor body
        parseBlock();
        require("}");
        init = scope_node->inputs;
    }
    // Check that all fields are initialized
    for (int i = idx; i < init.size(); i++) {
        if (init[i]->type_ == Type::TOP()) {
            error(tmp->obj_->name_ + " is not fully initialized, field '" + fs[i-idx]->fname_ + "' needs to be set in a constructor");
        }
    }
    Node *ptr = newStruct(tmp->obj_, con(tmp->obj_->offset(static_cast<int>(fs.size()))), idx, init);
    if (hasConstdructor) {
        scope_node->pop();
    }
    return ptr;
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
    while (isdigit(c = nextChar())) {}
    if (!(c == 'e' || c == '.')) {
        return --position - old;
    }
    while (isdigit(c = nextChar()) || c == 'e' || c == '.') {}
    return -(--position - old);
}

bool Lexer::matchOpx(char c0, char c1) {
    skipWhiteSpace();
    if(position + 1 >= input.size() || input[position] != c0 || input[position +1] == c1) return false;
    position++;
    return true;
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
