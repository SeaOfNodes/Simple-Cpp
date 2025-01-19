#ifndef LOGICAL_H
#define LOGICAL_H
#include "../../Include/node/logical_node.h"

#include "../../Include/parser.h"

class LogicalNode : public Node {
    // Source location for late reported errors
    Lexer loc_;
public:
    LogicalNode(Lexer loc, Node*lhs, Node*rhs);
    virtual std::string op() = 0;
    std::ostringstream &print_1(std::ostringstream &builder, Tomi::Vector<bool> &visit) override;
    // Todo: Parser.ParseException comes here
}

#endif