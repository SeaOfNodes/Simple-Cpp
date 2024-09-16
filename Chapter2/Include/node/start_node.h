#ifndef START_NODE_H
#define START_NODE_H

#include "../../Include/node/node.h"

class StartNode : public Node {
public:
  StartNode();
  [[nodiscard]] bool isCFG() const override;
  std::ostringstream &print_1(std::ostringstream &builder) override;

  std::string label() override;
  Type *compute() override;
  Node *idealize() override;
};

#endif