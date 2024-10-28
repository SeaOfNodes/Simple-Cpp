#ifndef MULTI_NODE_H
#define MULTI_NODE_H
#include "node.h"

class MultiNode : public Node {
public:
  MultiNode(std::initializer_list<Node *> = {});
  std::string label() override;
  std::ostringstream &print_1(std::ostringstream &builder, std::vector<bool>& visited) override;
};
#endif