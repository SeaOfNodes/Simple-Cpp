#ifndef MULTI_NODE_H
#define MULTI_NODE_H
#include "node.h"

// forward declare
class ProjNode;

class MultiNode : public Node {
public:
  MultiNode(std::initializer_list<Node *> = {});
  std::string label() override;
  std::ostringstream &print_1(std::ostringstream &builder, Tomi::Vector<bool>& visited) override;
  ProjNode* proj(int idx);
};
#endif