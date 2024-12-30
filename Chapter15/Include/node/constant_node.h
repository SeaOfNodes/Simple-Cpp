#ifndef CONSTANT_NODE_H
#define CONSTANT_NODE_H

#include "node.h"

// forward declare
class CFGNode;

class ConstantNode : public Node {
public:
    ConstantNode() = default;
    ConstantNode(Type *type, CFGNode *START);
    std::ostringstream builder;
  Type *con_;
  bool isMultiTail() override;
  bool isPinned() override;

  std::ostringstream &print_1(std::ostringstream &builder,
                              Tomi::Vector<bool>& visited) override;
  std::string label() override;
  std::string uniqueName() override;
  Type *compute() override;
  Node *idealize() override;
  bool eq(Node*) override;
  int hash() override;
};
#endif