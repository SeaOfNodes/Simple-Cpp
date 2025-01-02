#include "../../Include/node/node.h"
#include "../../Include/node/constant_node.h"

#include "../../Include/IR_printer.h"
#include "../../Include/parser.h"
#include "../../Include/utils.h"

#include "../../Include/type/type.h"
#include "../../Include/type/type_float.h"
#include "../../Include/node/to_float_node.h"

#include <typeinfo> // For typeid

Node::Node(std::initializer_list<Node *> inputNodes) {
    nid = UNIQUE_ID++;
     for (Node *n: inputNodes) {
        inputs.push_back(n);
        if (n != nullptr) {
            n->outputs.push_back(this);
        }
    }
}

Node::Node(Tomi::Vector<Node *> inputs_) {
    nid = UNIQUE_ID++;
    for (Node *n: inputs_) {
        inputs.push_back(n);
        if (n != nullptr) {
            n->outputs.push_back(this);
        }
    }
}

Node *Node::in(std::size_t i) const {
    return (i < inputs.size()) ? inputs[i] : nullptr;
}

Node *Node::out(std::size_t i) const {
    return (i < outputs.size()) ? outputs[i] : nullptr;
}

std::string Node::uniqueName() {
    // Todo: Get rid of $ as graphviz doesn't like it
    return label() + std::to_string(nid);
}

std::string Node::glabel() { return label(); }

std::size_t Node::nIns() const { return inputs.size(); }

Node *Node::keep() { KEEP++; return addUse(nullptr); }

Node *Node::unkeep(){
    UNKEEP++;
    delUse(nullptr);
    return this;
}

int Node::UID() {
    return UNIQUE_ID;
}

std::string Node::ToString() {
    std::ostringstream builder;
    return print(builder).str();
}

std::ostringstream &Node::print_0(std::ostringstream &builder,
                                  Tomi::Vector<bool> &visited) {
    if (nid >= visited.size()) {
        visited.resize(nid + 1, false);
    }

    if (visited[nid] && !(dynamic_cast<ConstantNode *>(this))) {
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

CFGNode *Node::cfg0() {
    return dynamic_cast<CFGNode *>(in(0));
}

std::ostringstream &Node::print(std::ostringstream &b) {
    print_0(b, bitset);
    return b;
}

std::size_t Node::nOuts() const { return outputs.size(); }

void Node::unlock() {
    if (hash_ == 0)
        return;
    GVN.remove(this);

    hash_ = 0;
}

Node* Node::copyF() {
    return nullptr;
}
Node* Node::widen() {
    if(!hasFloatInput()) return this;
    Node*flt = copyF();
    if(flt == nullptr) return this;
    // ignore control node
    for(int i =1; i < nIns(); i++) {
        auto*a = dynamic_cast<TypeFloat*>(in(i)->type_);
        flt->setDef(i, a ? in(i) : (alloc.new_object<ToFloatNode>(in(i)))->peephole());
    }
    kill();
    return flt;
}

bool Node::hasFloatInput() {
    for(int i =1; i < nIns(); i++) {
        if(dynamic_cast<TypeFloat*>(in(i)->type_)) {
            return true;
        }
    }
    return false;
}
Node *Node::setDef(int idx, Node *new_def) {
    unlock();
    Node *old_def = in(idx);
    if (old_def == new_def)
        return new_def;
    if (new_def != nullptr)
        new_def->addUse(this);
    inputs[idx] = new_def;

    if (old_def != nullptr && old_def->delUse(this))
        old_def->kill();

    moveDepsToWorkList();
    return new_def;
}

Node *Node::addUse(Node *n) {
    outputs.push_back(n);
    return this;
}

bool Node::iskeep() {
    return std::find(outputs.begin(), outputs.end(), nullptr) != outputs.end();
}

Node *Node::addDep(Node *dep) {
    if (std::find(deps_.begin(), deps_.end(), dep) != deps_.end())
        return this;
    if (std::find(inputs.begin(), inputs.end(), dep) != inputs.end())
        return this;
    if (std::find(outputs.begin(), outputs.end(), dep) != outputs.end())
        return this;
    deps_.push_back(dep);
    return this;
}

Node *Node::addDef(Node *new_def) {
    unlock();
    inputs.push_back(new_def);
    if (new_def != nullptr)
        new_def->addUse(this);
    return new_def;
}

bool Node::delUse(Node *use) {
    Utils::delVal(outputs, use);
//    auto it = std::find(outputs.begin(), outputs.end(), use);
//    if (it != outputs.end()) {
//        outputs.erase(it);
//    }
    return outputs.empty();
}

bool Node::isMultiHead() { return false; }

bool Node::isMultiTail() { return false; }

void Node::subsume(Node *nnn) {
    assert(nnn != this);
    while (nOuts() > 0) {
        Node *n = outputs.back();
        // N should not be null, as that means that the node you try to subsume is still in a keep-unkeep block.
        assert(n != nullptr);
        outputs.pop_back();
        n->unlock();
        auto it = std::find(n->inputs.begin(), n->inputs.end(), this);

        if (it != n->inputs.end()) {
            *it = nnn;
        }
        nnn->addUse(n);
    }
    kill();
}

Type *Node::setType(Type *type) {
    Type *old = type_;
    // Todo: Monotonicity should hold here.
    assert(old == nullptr || type->isa(old));
    if (old == type)
        return old;
    type_ = type;
    IterPeeps::addAll(outputs);
    moveDepsToWorkList();
    return old;
}

void Node::moveDepsToWorkList() {
    if (deps_.empty())
        return;
    IterPeeps::addAll(deps_);
    deps_.clear();
}

Node *Node::peepholeOpt() {
    ITER_CNT++;
    // end loop
    Type *inner = compute();
    Type *old = setType(inner);

    // Replace constant computations from non-constants with a constant node
    auto *a = dynamic_cast<ConstantNode *>(this);
    auto* b = dynamic_cast<XCtrlNode *>(this);
    if (!(a) && !(b) && type_->isHighOrConst()) {
        if (type_ == Type::XCONTROL()) {
            return alloc.new_object<XCtrlNode>();
        }
        auto peepholedNode = (alloc.new_object<ConstantNode>(type_, Parser::START))->peepholeOpt();

        return peepholedNode;
    }
    // Global Value Numbering
    if (hash_ == 0) {
        // calls HashCode for hashing
        Node **nPtr = GVN.get(this); // Will set _hash as a side effect
        if (nPtr == nullptr) {
            GVN.put(this, this);
        } else {
            Node *n = *nPtr;
            // Because of random worklist ordering, the two equal nodes
            // might have different types.  Because of monotonicity, both
            // types are valid.  To preserve monotonicity, the resulting
            // shared Node has to have the best of both types.
            n->setType(n->type_->join(type_));
            hash_ = 0;
            return deadCodeElim(n);
        }
    }

    Node *n = idealize();

    if (n != nullptr) {
        return n;
    }


    if (old == type_)
        ITER_NOP_CNT++;
    // returns this if type is mutated
    return old == type_ ? nullptr : this;
}

bool Node::test_nid(int i) {
    bool test = false;
    for (auto in: inputs) {
        if (in != nullptr) {
            if (in->nid == i) test = true;
        }
    }
    if (!test) {
        return false;
    } else {
        return true;
    }
}

Node *Node::peephole() {
    if (disablePeephole) {
        type_ = compute(); // assign type_ to con_
        return this;
    }
    Node *n = peepholeOpt();

    return n == nullptr ? this : deadCodeElim(n->peephole()->keep())->unkeep();
}

/*
 * If the scope gets popped, then delete
 * each of the individual elements.
 * */
void Node::popN(std::size_t n) {
    unlock();
    for (int i = 0; i < n; i++) {
        Node *old_def = inputs.back();
        inputs.pop_back();
        if (old_def != nullptr && old_def->delUse(this))
            old_def->kill();
    }
}

bool Node::allCons(Node *dep) {
    for (int i = 1; i < nIns(); i++) {
        if (!((in(i))->type_->isConstant())) {
            in(i)->addDep(
                    dep); // If in(i) becomes a constant later, will trigger some peephole
            return false;
        }
    }
    return true;
}

// Todo: use DelVal here
Node *Node::delDef(int idx) {
    unlock();
    Node **old_def = &inputs[idx];
    Node *nptr = *old_def;
    Node *tmp = inputs.back();
    inputs.pop_back();
    inputs[idx] = tmp;
    if (// If the old def exists, remove a def->use edge
        nptr->delUse(
                this))     // If we removed the last use, the old def is now dead
        nptr->kill(); // Kill old def


    // erase is bad here
    return this;
}

Node *Node::copy(Node *lhs, Node *rhs) {
    throw std::runtime_error("Binary ops need to implement copy!");
}

bool Node::isUnused() const { return outputs.empty(); }

bool Node::isCFG() { return false; }

bool Node::isMem() {
    return false;
}

Node *Node::getBlockStart() {
    return nullptr;
}

bool Node::isPinned() {
    return false;
}

void Node::kill() {
    unlock();
    moveDepsToWorkList();
    assert(isUnused()); // has no uses so it is dead
    type_ = nullptr;
    while (nIns() >
           0) { // Set all inputs to null, recursively killing unused Nodes
        Node *old_def = inputs.back();
        inputs.pop_back();
        if (old_def != nullptr) {
            IterPeeps::add(old_def);
            if (old_def->delUse(this))
                old_def->kill();
        }
    }
    /*
      popN(nIns());
      inputs.clear(); // flag as dead
    */

    assert(isDead()); // Really dead now
}

void Node::reset() {
    UNIQUE_ID = 1;
    disablePeephole = false;
    // clear the hashmap
    ITER_CNT = ITER_NOP_CNT = 0;
}

int Node::UNIQUE_ID = 1;

int Node::KEEP  = 0;
int Node::UNKEEP = 0;

bool Node::isDead() { return isUnused() && nIns() == 0 && type_ == nullptr; }

bool Node::disablePeephole = false;

Node *Node::find(Tomi::Vector<bool> visit, int nid_) {
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

bool Node::eq(Node *n) { return true; }

std::string Node::err() {
    return "";
}

Node *Node::idealize() { return nullptr; }

Type *Node::compute() { return nullptr; }

Node *Node::deadCodeElim(Node *m) {
    if (m != this && isUnused() && !isDead()) {
        m->keep();
        kill();
        m->unkeep();
    }
    return m;
}

Node *Node::swap12() {
    unlock();
    Node *tmp = in(1);
    inputs[1] = in(2);
    inputs[2] = tmp;
    return this;
}

int Node::hash() { return 0; }

unsigned long long Node::hashCode() {
    if (hash_ != 0)
        return hash_;
    int hashc = hash();
    for (Node *n: inputs) {
        if (n != nullptr) {
            hashc = hashc ^ (hashc << 17) ^ (hashc >> 13) ^ n->nid;
        }
    }
    // if hash is still zero
    if (hashc == 0)
        hashc = 0xDEADBEEF;
    hash_ = hashc;
    return hash_;
}

Tomi::HashMap<Node *, Node *> Node::GVN = Tomi::HashMap<Node *, Node *>();

// Part of hashmap interface see in header.
unsigned long long Tomi::hash<Node *>::operator()(Node *val) {
    return val->hashCode();
}

bool Node::operator==(Node &o) {
    if (&o == this)
        return true;
    if (typeid(o) != typeid(*this))
        return false;
    size_t len = inputs.size();
    if (len != o.inputs.size())
        return false;
    for (int i = 0; i < len; i++) {
        if (in(i) != o.in(i))
            return false;
    }
    return eq(&o);
}
// Todo: should not be a template OR should be defined in the header

std::string Node::p(int depth) {
    return IRPrinter::prettyPrint(this, depth);
}

Tomi::BitArray<10> Node::WVISIT = Tomi::BitArray<10>();
int Node::ITER_NOP_CNT = 0;
int Node::ITER_CNT = 0;
