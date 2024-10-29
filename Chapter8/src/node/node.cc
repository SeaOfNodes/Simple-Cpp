#include "../../Include/node/node.h"
#include "../../Include/node/constant_node.h"

Node::Node(std::initializer_list<Node *> inputNodes) {
  nid = UNIQUE_ID++;
  for (Node *n : inputNodes) {
    inputs.push_back(n);
    if (n != nullptr) {
      n->outputs.push_back(this);
    }
  }
}

Node::Node(std::vector<Node *> inputs_) {
  nid = UNIQUE_ID++;
  for (Node *n : inputs_) {
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

Node *Node::keep() { return addUse(nullptr); }

Node *Node::unkeep() {
  delUse(nullptr);
  return this;
}

std::string Node::to_string() {
  std::ostringstream builder;
  return print(builder).str();
}

std::ostringstream &Node::print_0(std::ostringstream &builder,
                                  std::vector<bool> &visited) {
  if (nid >= visited.size()) {
    visited.resize(nid + 1, false);
  }

  if (visited[nid]) {
    builder << label();
    return builder;
  }

  visited[nid] = true;

  if (isDead()) {
    builder << uniqueName() << ": DEAD";
    return builder;
  } else {
    return print_1(builder, visited);
  }
}

std::ostringstream &Node::print(std::ostringstream &b) {
  print_0(b, bitset);
  return b;
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

Node *Node::addDef(Node *new_def) {
  inputs.push_back(new_def);
  if (new_def != nullptr)
    new_def->addUse(this);
  return new_def;
}

bool Node::delUse(Node *use) {
  auto it = std::find(outputs.begin(), outputs.end(), use);
  outputs.erase(it);
  return outputs.empty();
}

bool Node::isMultiHead() { return false; }
bool Node::isMultiTail() { return false; }
void Node::subsume(Node *nnn) {
  assert(nnn != this);
  while (nOuts() > 0) {
    Node *n = outputs.back();
    outputs.pop_back();
    auto it = std::find(n->inputs.begin(), n->inputs.end(), this);

    if (it != n->inputs.end()) {
      *it = nnn;
    }
    nnn->addUse(n);
  }
  kill();
}
Node *Node::peephole() {
  Type *type = compute();
  type_ = type; // assign type_ to con_

  if (disablePeephole)
    return this;

  auto *a = dynamic_cast<ConstantNode *>(this);
  // If type is constant replace it with a constant nodex
  if (!(a) && type_->isConstant()) {
    // Create the ConstantNode object and call peephole() on it
    auto peepholedNode = (new ConstantNode(type, Parser::START))->peephole();

    // Pass the result of peephole() to deadCodeElim
    return deadCodeElim(peepholedNode);
  }
  Node *n = idealize();
  if (n != nullptr)
    return deadCodeElim(n->peephole());
  return this;
}

/*
 * If the scope gets popped, then delete
 * each of the individual elements.
 * */
void Node::popN(std::size_t n) {
  for (int i = 0; i < n; i++) {
    Node *old_def = inputs.back();
    inputs.pop_back();
    if (old_def != nullptr && old_def->delUse(this))
      ;
  }
}

bool Node::allCons() {
  for (int i = 1; i < nIns(); i++) {
    if (!((in(i))->type_->isConstant()))
      return false;
  }
  return true;
}
Node *Node::idom() {
  Node *idom = in(0);
  if (idom->i_depth == 0)
    idom->idom(); // Recursively set _idepth
  if (i_depth == 0)
    i_depth = idom->i_depth + 1;
  return idom;
}

void Node::delDef(int idx) {
  Node *old_def = in(idx);
  if (old_def != nullptr && // If the old def exists, remove a def->use edge
      old_def->delUse(
          this))     // If we removed the last use, the old def is now dead
    old_def->kill(); // Kill old def
  inputs.erase(std::next(inputs.begin(), idx));
}
Node *Node::copy(Node *lhs, Node *rhs) {
  throw std::runtime_error("Binary ops need to implement copy!");
}

bool Node::isUnused() const { return outputs.empty(); }
bool Node::isCFG() { return false; }

void Node::kill() {
  assert(isUnused()); // has no uses so it is dead
  popN(nIns());
  inputs.clear(); // flag as dead
  type_ = nullptr;
  assert(isDead()); // Really dead now
}

void Node::reset() {
  UNIQUE_ID = 1;
  disablePeephole = false;
}

int Node::UNIQUE_ID = 1;
bool Node::isDead() { return isUnused() && nIns() == 0 && type_ == nullptr; }

bool Node::disablePeephole = false;

Node *Node::find(std::vector<bool> visit, int nid_) {
  if (nid == nid_)
    return this;
  if (visit[nid])
    return nullptr;
  visit[nid] = true;

  // Traverse inputs
  for (Node *def : inputs) {
    if (def != nullptr) {
      Node *rez = def->find(visit, nid);
      if (rez != nullptr)
        return rez;
    }
  }

  // Traverse outputs
  for (Node *use : outputs) {
    Node *rez = use->find(visit, nid);
    if (rez != nullptr)
      return rez;
  }
  return nullptr;
}

Node *Node::idealize() { return nullptr; }
Type *Node::compute() { return nullptr; }

Node *Node::deadCodeElim(Node *m) {
  if (m != this && isUnused()) {
    m->keep();
    kill();
    m->unkeep();
  }
  return m;
}

Node *Node::swap12() {
  Node *tmp = in(1);
  inputs[1] = in(2);
  inputs[2] = tmp;
  return this;
}
