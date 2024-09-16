#include "../../Include/node/node.h"

Node::Node(std::initializer_list<Node *> inputNodes) {
  for (Node *n : inputNodes) {
    inputs.push_back(n);
    if (n != nullptr) {
      n->outputs.push_back(this);
    }
  }
}
Node *Node::in(std::size_t i) const {
  return (i < inputs.size()) ? inputs[i] : nullptr;
}

std::string Node::uniqueName() { return label() + std::to_string(nid); }
std::string Node::glabel() { return label(); }
std::string Node::label() { return "Node"; }

std::size_t Node::nIns() const { return inputs.size(); }

std::size_t Node::nOuts() const { return outputs.size(); }

bool Node::isUnused() const { return outputs.empty(); }
bool Node::isCFG() const { return false; }
void Node::reset() { UNIQUE_ID = 1; }
int Node::UNIQUE_ID = 1;