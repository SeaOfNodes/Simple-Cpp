#ifndef MINUS_NODE_H
#define MINUS_NODE_H

#include "../../Include/node/node.h"

class MinusNode : public Node {
public:
  MinusNode(Node *in);
  std::string label();
  std::string glabel();

  std::ostringstream &print_1(std::ostringstream &, std::vector<bool> visited);

  Type *compute();
  Node *idealize();
};

#endif