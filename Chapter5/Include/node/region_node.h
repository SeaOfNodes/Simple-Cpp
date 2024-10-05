#ifndef REGION_NODE_H
#define REGION_NODE_H

#include "node.h"
#include <initializer_list>

class RegionNode : public Node {
public:
  RegionNode(std::initializer_list<Node *> nodes);
  std::string label() override;
  std::ostringstream &print_1(std::ostringstream &builder) override;
  bool isCFG() const;

  Type *compute() override;
  Node *idealize() override;
};
#endif