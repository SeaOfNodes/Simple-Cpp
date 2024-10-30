#include "../Include/Iter_peeps.h"

template <typename T> IterPeeps::WorkList<T>::WorkList() : WorkList(123) {}

template <typename T>
IterPeeps::WorkList<T>::WorkList(long seed)
    : es_(1, 0), len_(0), seed(seed), totalWork(0), rng_(seed) {}

template <typename T> T IterPeeps::WorkList<T>::push(T x) {
  if (x == nullptr)
    return nullptr;
  int idx = x.nid;
  if (!onV[idx]) {
    onV[idx] = true;
    if (len_ == es_.size()) {
      es_.resize(len_ << 1);
    }
    es_[len_++] = x;
    totalWork++;
  }
  return x;
}

template <typename T> T IterPeeps::WorkList<T>::addAll(Tomi::Vector<T> e) {
  for (T n : e) {
    push(n);
  }
}

template <typename T> T IterPeeps::WorkList<T>::on(T x) { return onV[x.nid]; }

template <typename T> T IterPeeps::WorkList<T>::pop() {
  if (len_ == 0)
    return nullptr;
  int idx = std::uniform_int_distribution<int>(0, len_ - 1)(rng_);
  T x = es_[idx];
  es_[idx] = es_[--len_];
  onV[idx] = false;
}

template <typename T> T IterPeeps::WorkList<T>::clear() {
  len_ = 0;
  rng_.seed(seed);
  std::fill(onV.begin(), onV.end(), false);
  totalWork = 0;
}

template <typename T>
T IterPeeps::add(T n) {
  return WORK.push(n);
}

void IterPeeps::addAll(Tomi::Vector<Node *> ary) {
  WORK.addAll(ary);
}

StopNode *IterPeeps::iterate(StopNode *stop, bool show) {

}

bool IterPeeps::progressOnList(Node *stop) {
  MID_ASSERT = true;
  int old_cnt = Node::ITER_CNT;
}