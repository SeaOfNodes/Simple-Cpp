#include "../../Include/node/constant_node.h"

ConstantNode::ConstantNode(long value, Node* START) : value_(value), Node({START}){}