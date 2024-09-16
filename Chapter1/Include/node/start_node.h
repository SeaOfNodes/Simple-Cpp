#ifndef START_NODE_H
#define START_NODE_H

#include "../../Include/node/node.h"

class StartNode : public Node{
public:
  StartNode();
  bool isCFG() const override;

};

#endif