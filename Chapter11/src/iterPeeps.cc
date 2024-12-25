#include "../Include/graph_visualizer.h"
#include "../Include/globalCodeMotion.h"
#include "../Include/node/node.h"
#include "../Include/node/stop_node.h"

IterPeeps::WorkList::WorkList() : WorkList(123) {}

IterPeeps::WorkList::WorkList(long seed)
  : seed(seed), totalWork(0), trackWork(0),  on_(), rng(seed) {
}

Node *IterPeeps::WorkList::push(Node *x) {
  if (x == nullptr)
    return nullptr;
  int idx = x->nid;
  if (!on_.test(idx)) {
    on_.set(idx);
    es.push_back(x);
    totalWork++;
    trackWork++;
  }
  return x;
}

void IterPeeps::WorkList::addAll(Tomi::Vector<Node *> e) {
  for (auto n : e) {
    push(n);
  }
}

bool IterPeeps::WorkList::on(Node *x) { return on_.test(x->nid); }

Node *IterPeeps::WorkList::pop() {
  if (es.empty())
    return nullptr;
  std::uniform_int_distribution<size_t> gen(0, es.size() - 1);
  size_t idx = gen(rng);
  Node **x = &es[idx];
  auto* Ptr = *x;
  on_.reset(Ptr->nid);
  es.erase(x); // compress raray making sure algorithm will terminate
  trackWork--;
  return Ptr;
}

void IterPeeps::WorkList::clear() {
  on_.reset();

  totalWork = 0;
  trackWork = 0;
}

void IterPeeps::reset() { WORK.clear(); }

Node *IterPeeps::add(Node *n) { return WORK.push(n); }

void IterPeeps::addAll(Tomi::Vector<Node *> ary) { WORK.addAll(ary); }

IterPeeps::WorkList IterPeeps::WORK = IterPeeps::WorkList();

StopNode *IterPeeps::iterate(StopNode *stop, bool show) {
  /*   assert(progressOnList(stop));*/
  int cnt{};
  Node *n;
  while (true) {
     n = WORK.pop();
  if(n == nullptr) break;


    if (n->isDead())
      continue;
    cnt++;
    Node *x = n->peepholeOpt();

    if (x != nullptr) {
      if (x->isDead())
        continue;
      // peepholeOpt can return brand-new nodes, needing an initial type set
      if (x->type_ == nullptr)
        x->setType(x->compute());
      // Changes require neighbors onto the worklist
      if (x != n || !(dynamic_cast<ConstantNode *>(x))) {
        // All outputs of n (changing node) not x (prior existing node).
        for (Node *z : n->outputs) {
            WORK.push(z);
        }
        // Everybody gets a free "go again" in case they didn't get
        // made in their final form.
        WORK.push(x);
        // If the result is not self, revisit all inputs (because
        // there's a new user), and replace in the graph.
        if (x != n) {
          for (Node *z : n->inputs) {
              WORK.push(z);
          }
          n->subsume(x);
        }
      }
      // If there are distant neighbors, move to worklist
      n->moveDepsToWorkList();
    }
    if(n->isUnused() && !(dynamic_cast<StopNode*>(n))) {
        n->kill(); // just plain dead
    }
  }
  if (show) {
    /*std::cerr << (new GraphVisualiser().generateDotOutput(stop, nullptr,
     * nullptr));*/
  }
  GlobalCodeMotion::buildCFG(stop);
  return stop;
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
