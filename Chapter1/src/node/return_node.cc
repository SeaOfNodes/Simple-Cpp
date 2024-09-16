#include "../../Include/node/return_node.h"

ReturnNode::ReturnNode(Node* ctrl, Node* data) : Node({ctrl, data}){}

[[nodiscard ]] Node* ReturnNode::ctrl() {return in(0); }
[[nodiscard ]] Node* ReturnNode::expr() {return in(1); }


bool ReturnNode::isCFG() const {return true;}