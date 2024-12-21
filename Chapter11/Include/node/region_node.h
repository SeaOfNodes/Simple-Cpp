#ifndef REGION_NODE_H
#define REGION_NODE_H

#include "node.h"
#include "phi_node.h"

#include <initializer_list>

class RegionNode : public CFGNode {
public:
  RegionNode(std::initializer_list<Node *> nodes);
  std::string label() override;
  std::ostringstream &print_1(std::ostringstream &builder, Tomi::Vector<bool>& visited) override;
  bool isCFG() override;

  int findDeadInput();
  Type *compute() override;
  Node *idealize() override;
  Node *idom_; // Immediate dominator cache
  // Immediate dominator of Region is a little more complicated.
  int idepth() override;
  CFGNode *idom() override;
  bool isMultiHead() override;
  // True if last input is null
  bool blockHead() override;
  bool inProgress();
  bool hashPhi();
  bool eq(Node* n) override;
  void walkUnreach_(Tomi::BitArray<10> &vitisted, Tomi::HashMap<CFGNode *, CFGNode *> unreach) override;
  int loopDepth() override;
  Node* getBlockStart();

};

class LoopNode : public RegionNode {
public:
  LoopNode(Node* entry);
  Node* entry();
  Node* back();
  std::string label() override;
  Type *compute() override;
  Node *idealize() override;
  CFGNode *idom() override;
  void forceExit(StopNode* stop);
  int loopDepth() override;
  int idepth() override;
};
#endif