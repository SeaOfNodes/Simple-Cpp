#include "../../Include/node/node.h"
#include "../../Include/node/constant_node.h"
#include <sstream>

Node::Node(std::initializer_list<Node *> inputNodes) {
  nid = ++UNIQUE_ID;
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
std::size_t Node::nIns() const { return inputs.size(); }

std::ostringstream &Node::print_0(std::ostringstream &builder) {
  if (isDead()) {
    builder << uniqueName() << ": DEAD";
    return builder;
  } else {
    return print_1(builder);
  }
}

std::ostringstream &Node::print() {
  std::ostringstream builder;
  return print_0(builder);
}

std::size_t Node::nOuts() const { return outputs.size(); }

Node *Node::setDef(int idx, Node *new_def) {
  Node *old_def = in(idx);
  if (old_def == new_def)
    return this;
  if (new_def != nullptr)
    new_def->addUse(this);
  if (old_def != nullptr && old_def->delUse(this))
    old_def->kill();
  inputs[idx] = new_def;
  return new_def;
}

Node *Node::addUse(Node *n) {
  outputs.push_back(n);
  return this;
}

bool Node::delUse(Node *use) {
  auto it = std::find(outputs.begin(), outputs.end(), use);
  outputs.erase(it);
  return outputs.empty();
}

Node *Node::peephole() {
  Type *type = compute();
  type_ = type; // assign type_ to con_

  if (disablePeephole)
    return this;

  auto *a = dynamic_cast<ConstantNode *>(this);
  // If type is constant replace it with a constant node
  if (!(a) && type_->isConstant()) {
    kill();
    return (new ConstantNode(type, Parser::START))->peephole();
  }
  Node *n = idealize();
  if (n != nullptr)
    return n;
  return this;
}
bool Node::isUnused() const { return outputs.empty(); }
bool Node::isCFG() const { return false; }
void Node::kill() {
  assert(isUnused()); // has no uses so it is dead
  for (int i = 0; i < nIns(); i++) {
    setDef(i,
           nullptr); // Set all inputs to null, recursively killing unused Nodes
  }
  inputs.clear(); // flag as dead
  type_ = nullptr;
  assert(isDead()); // Really dead now
}
void Node::reset() { UNIQUE_ID = 1; }
int Node::UNIQUE_ID = 1;
bool Node::isDead() { return isUnused() && nIns() == 0 && type_ == nullptr; }

bool Node::disablePeephole = false;

Node *Node::idealize() { return nullptr; }
Type *Node::compute() { return nullptr; }
