#ifndef MULTI_NODE_H
#define MULTI_NODE_H
#include "cfg_node.h"

// forward declare
class ProjNode;
class CProjNode;

class MultiNode : public CFGNode {
public:
  MultiNode(std::initializer_list<Node *> = {});
  std::string label() override;
  std::ostringstream &print_1(std::ostringstream &builder, Tomi::Vector<bool>& visited) override;
  ProjNode* proj(int idx);
  CProjNode* cproj(int idx);

};
#endif