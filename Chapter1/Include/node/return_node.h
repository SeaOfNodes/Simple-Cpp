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

  [[nodiscard]] bool isCFG() const override;
};
#endif