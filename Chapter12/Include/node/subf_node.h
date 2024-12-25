#ifndef SUBF_NODE_H
#define SUBF_NODE_H

#include "../../Include/node/node.h"

class SubfNode : public Node {
public:
    SubfNode(Node *lhs, Node *rhs);
  std::string label();
  std::string glabel();

  std::ostringstream &print_1(std::ostringstream &builder, Tomi::Vector<bool>& visited) override;

  Type *compute();
  Node *idealize();
  Node* copyF();
};

#endif