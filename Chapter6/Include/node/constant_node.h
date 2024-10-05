#ifndef CONSTANT_NODE_H
#define CONSTANT_NODE_H

#include "../../Include/parser.h"
#include "node.h"

class ConstantNode : public Node {
public:
  std::ostringstream builder;
  ConstantNode(Type *type, Node *START);
  Type *con_;
  std::ostringstream &print_1(std::ostringstream &builder);
  std::string label() override;
  std::string uniqueName();
  Type *compute();
  Node *idealize();
};
#endif