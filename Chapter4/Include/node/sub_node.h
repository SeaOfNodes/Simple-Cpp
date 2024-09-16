#ifndef SUB_NODE_H
#define SUB_NODE_H

#include "../../Include/node/node.h"

class SubNode: public Node {
public:
SubNode(Node*lhs, Node*rhs);
std::string label();
std::string glabel();

std::ostringstream& print_1(std::ostringstream&);

Type*compute();
Node* idealize();
};
#endif