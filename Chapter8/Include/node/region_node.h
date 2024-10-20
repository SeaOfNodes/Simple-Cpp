#ifndef REGION_NODE_H
#define REGION_NODE_H

#include "node.h"
#include "phi_node.h"

#include <initializer_list>

class RegionNode : public Node {
public:
  RegionNode(std::initializer_list<Node *> nodes);
  std::string label() override;
  std::ostringstream &print_1(std::ostringstream &builder, std::vector<bool> visited) override;
  bool isCFG() override;

  int findDeadInput();
  Type *compute() override;
  Node *idealize() override;
  Node *idom_; // Immediate dominator cache
  // Immediate dominator of Region is a little more complicated.
  Node *idom() override;
  bool isMultiHead() override;
  // True if last input is null
  bool inProgress();
};
#endif