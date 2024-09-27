#ifndef IF_NODE_H
#define IF_NODE_H
#include "../type/tuple_type.h"
#include "multi_node.h"
#include "node.h"

class IfNode : public MultiNode {
public:
  IfNode(Node *ctrl, Node *parent);
  std::string label() override;
  std::ostringstream &print_1(std::ostringstream &builder) override;

  bool isCFG() const override;
  Node *ctrl();

  Node *pred();

  Type *compute() override;
  Node *idealize() override;
};
#endif