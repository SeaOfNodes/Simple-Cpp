#ifndef IF_NODE_H
#define IF_NODE_H
#include "../../Include/node/proj_node.h"
#include "../../Include/type/integer_type.h"
#include "../type/tuple_type.h"

#include "multi_node.h"
#include "node.h"

class IfNode : public MultiNode {
public:
  IfNode(Node *ctrl, Node *parent);
  std::string label() override;
  std::ostringstream &print_1(std::ostringstream &builder,
                              std::vector<bool> visited) override;

  bool isCFG() override;
  Node *ctrl();
  Node *pred();
  bool isMultiHead() override;

  Type *compute() override;
  Node *idealize() override;
};
#endif