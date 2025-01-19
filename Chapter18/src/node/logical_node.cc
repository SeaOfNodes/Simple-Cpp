#include "../../Include/node/logical_node.h"

LogicalNode::LogicalNode(Lexer loc, Node *lhs, Node *rhs): Node({nullptr, lhs, rhs}), loc_(loc) {

}

std::ostringstream &LogicalNode::print_1(std::ostringstream &builder, Tomi::Vector<bool> &visit) {
    builder << "(";
    in(1)->print_0(builder, visited);
    builder << op();
    in(2)->print_0(builder, visited);
    builder << ")";
    return builder;
}
// Todo::Parser.ParseException comes here