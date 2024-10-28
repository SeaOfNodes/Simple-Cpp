#ifndef RETURN_NODE_H
#define RETURN_NODE_H
#include "../../Include/node/node.h"

class ReturnNode: public Node {
public:
  // ctrl - predecessor control node
  // data - Data node value
  ReturnNode(Node* ctrl, Node* data);
  Node* ctrl();
  Node* expr();
  std::ostringstream& print_1(std::ostringstream& builder, std::vector<bool>& visited) override;
  [[nodiscard]] bool isCFG() override;

  std::string label() override;
  Type*compute() override;
  Node*idealize() override;
};
#endif