#ifndef MUL_NODE_H
#define MUL_NODE_H

#include "../../Include/node/add_node.h"
#include "../../Include/node/node.h"

class MulNode : public Node {
public:
  MulNode(Node *lhs, Node *rhs);
  std::string label();
  std::string glabel();

  std::ostringstream &print_1(std::ostringstream &);

  Type *compute() override;
  Node *idealize() override;
  Node *copy(Node *lhs, Node *rhs) override;
};
#endif