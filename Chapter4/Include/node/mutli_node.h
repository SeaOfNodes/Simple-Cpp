#ifndef MULTI_NODE_H
#define MULTI_NODE_H
#include "../../Include/node/node.h"

class MultiNode : public Node {
  MultiNode(std::initializer_list<Node*>);
};
#endif