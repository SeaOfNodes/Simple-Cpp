#ifndef DIV_NODE_H
#define DIV_NODE_H
#include "../../Include/node/node.h"

class DivNode : public Node {
public:
  DivNode(Node *lhs, Node *rhs);
  std::string label();
  std::string glabel();

  std::ostringstream &print_1(std::ostringstream &builder, std::vector<bool>& visited) override;

  Type *compute() override;
  Node *idealize() override;
  Node *copy(Node *lhs, Node *rhs) override;
};
#endif