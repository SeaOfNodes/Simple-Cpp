#ifndef STOP_NODE_H
#define STOP_NODE_H

#include "../../Include/node/node.h"
#include "../../Include/node/return_node.h"

class StopNode : public Node {
public:
  StopNode(std::initializer_list<Node*> inputs);
  std::string label() override;

  bool isCFG() const override;
  std::ostringstream &print_1(std::ostringstream &builder) override;

  // If a single Return, return it.
  // Otherwise, null because ambiguous.
  ReturnNode* ret();
  Type* compute();
  Node* idealize();

  Node* addReturn(Node* node);
};
#endif