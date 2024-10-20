#ifndef LOOP_NODE_H
#define LOOP_NODE_H

#include "../../Include/node/region_node.h"
class LoopNode : public RegionNode {
public:
  LoopNode(Node* entry);
  Node* entry();
  Node* back();
  std::string label() override;
  Type *compute() override;
  Node *idealize() override;
  Node *idom() override;
};
#endif