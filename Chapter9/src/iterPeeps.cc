#include "../Include/node/node.h"

IterPeeps::WorkList::WorkList() : WorkList(123) {
}

IterPeeps::WorkList::WorkList(long seed)
    : seed(seed), totalWork(0), rng(seed) {
}

template<typename T>
T IterPeeps::WorkList::push(T x) {
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

template<typename T>
void IterPeeps::WorkList::addAll(std::vector<T> e) {
    for (T n: e) {
        push(n);
    }
}

template<typename T>
bool IterPeeps::WorkList::on(T x) {
    return on_.test(x.nidA);
}

template<typename T>
T IterPeeps::WorkList::pop() {
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

template<typename T>
T IterPeeps::add(T n) { return WORK.push(n); }

void IterPeeps::addAll(std::vector<Node *> ary) { WORK.addAll(ary); }

IterPeeps::WorkList IterPeeps::WORK = IterPeeps::WorkList();

StopNode *IterPeeps::iterate(StopNode *stop, bool show) {
}

/*
bool IterPeeps::progressOnList(Node *stop) {
  MID_ASSERT = true;
  int old_cnt = Node::ITER_CNT;
}*/

bool IterPeeps::MidAssert() { return MID_ASSERT; }

bool IterPeeps::progressOnList(Node *stop) {
    MID_ASSERT = true;

    MID_ASSERT = false;
    return false;
}

bool IterPeeps::MID_ASSERT = false;
