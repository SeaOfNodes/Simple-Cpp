#include "../../Include/node/mutli_node.h"

MultiNode::MultiNode(std::initializer_list<Node *> nodes) : Node({nodes}) {}