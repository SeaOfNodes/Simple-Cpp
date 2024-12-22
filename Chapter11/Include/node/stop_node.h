#ifndef STOP_NODE_H
#define STOP_NODE_H

#include "../../Include/node/node.h"
#include "../../Include/node/return_node.h"

#include <initializer_list>
class StopNode : public CFGNode {
public:
  StopNode() {}
  StopNode(std::initializer_list<Node *> inputs);

  std::string label() override;

  bool blockHead() override;
  std::ostringstream &print_1(std::ostringstream &builder, Tomi::Vector<bool>& visited) override;

  // If a single Return, return it.
  // Otherwise, null because ambiguous.
  ReturnNode *ret();
  Type *compute() override;
  Node *idealize() override;
  CFGNode*idom() override;
  int idepth() override;
  StopNode* typeCheck();

  int loopDepth() override;
  Node *addReturn(Node *node);
  StopNode* iterate();
  StopNode* iterate(bool show);
  Node* getBlockStart() override;
};
#endif