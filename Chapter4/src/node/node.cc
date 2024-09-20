#include "../../Include/node/node.h"
#include "../../Include/node/constant_node.h"
#include <sstream>

Node::Node(std::initializer_list<Node *> inputNodes) {
    nid = ++UNIQUE_ID;
    for (Node *n: inputNodes) {
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

Node *Node::peephole() {
    Type *type = compute();
    type_ = type; // assign type_ to con_

    if (disablePeephole)
        return this;

    auto *a = dynamic_cast<ConstantNode *>(this);
    // If type is constant replace it with a constant node
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
void Node::popN(int n) {
    for (int i = 0; i < n; i++) {
        Node *old_def = inputs.back();
        inputs.pop_back();
        if (old_def != nullptr && old_def->delUse(this));
    }
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
    for (Node *def: inputs) {
        if (def != nullptr) {
            Node *rez = def->find(visit, nid);
            if (rez != nullptr)
                return rez;
        }
    }

    // Traverse outputs
    for (Node *use: outputs) {
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
