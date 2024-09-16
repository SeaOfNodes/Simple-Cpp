#ifndef START_NODE_H
#define START_NODE_H

#include "../../Include/node/node.h"

class StartNode : public Node {
public:
  StartNode();
  bool isCFG() const override;
  std::ostringstream &print_1(std::ostringstream &builder);

  Type *compute() override;
  Node *idealize() override;
  std::string label() override;
};

#endif