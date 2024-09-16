#ifndef CONSTANT_NODE_H
#define CONSTANT_NODE_H

#include "../../Include/node/node.h"
#include "../Include/parser.h"

class ConstantNode : public Node {
public:
  ConstantNode(long value, Node* START);

  long value_;
};
#endif