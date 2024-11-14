#include "../Include/node/node.h"
#include "../Include/node/stop_node.h"

IterPeeps::WorkList::WorkList() : WorkList(123) {
}

IterPeeps::WorkList::WorkList(long seed)
    : seed(seed), totalWork(0), rng(seed) {
}

T* IterPeeps::WorkList::push(Node* x) {
    if (x == nullptr)
        return nullptr;
    int idx = x->nid;
    if (!on_.test(idx)) {
        on_.set(idx);
        es.push_back(x);
        totalWork++;
    }
    return x;
}

void IterPeeps::WorkList::addAll(std::vector<Node*> e) {
    for (T n: e) {
        push(n);
    }
}

bool IterPeeps::WorkList::on(Node* x) {
    return on_.test(x.nidA);
}

Node* IterPeeps::WorkList::pop() {
    if (es.empty())
        return nullptr;
    std::uniform_int_distribution<int> gen(0, es.size());
    int idx = gen(rng);
    T x = es[idx];
    on_.reset(x.id);
    return x;
}

void IterPeeps::WorkList::clear() {
    on_.reset();

    totalWork = 0;
}

void IterPeeps::reset() { WORK.clear(); }

Node* IterPeeps::add(Node* n) { return WORK.push(n); }

void IterPeeps::addAll(std::vector<Node *> ary) { WORK.addAll(ary); }

IterPeeps::WorkList IterPeeps::WORK = IterPeeps::WorkList();

StopNode *IterPeeps::iterate(StopNode *stop, bool show) {
/*   assert(progressOnList(stop));*/
}

bool IterPeeps::MidAssert() { return MID_ASSERT; }

// Todo: Implement this later
/*
bool IterPeeps::progressOnList(StopNode *stop) {
    MID_ASSERT = true;
    int old_cnt = Node::ITER_CNT;
    int old_nop = Node::ITER_NOP_CNT;

    // inner lambda here
    Node* changed = stop->walk();
    Node::ITER_CNT = old_cnt;
    Node::ITER_NOP_CNT = old_nop;

    MID_ASSERT = false;
    return false;
}
*/

bool IterPeeps::MID_ASSERT = false;
