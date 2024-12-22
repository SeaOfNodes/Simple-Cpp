#include "../Include/IR_printer.h"
#include "../Include/node/node.h"
#include "../Include/node/region_node.h"

// Another bulk pretty-printer.  Makes more effort at basic-block grouping.
std::string IRPrinter::prettyPrint(Node *node, int depth) {
    BFS *bfs = new BFS(node, depth);
    // First, a Breadth First Search at a fixed depth.
    Tomi::Vector<Node *> rpos;
    Tomi::BitArray<10> visit;
    for (int i = bfs->lim_; i < bfs->_bfs.size(); i++) {
        postOrd(bfs->_bfs[i], rpos, visit, bfs->_bs);
    }
    // Reverse the post-order walk
    std::ostringstream builder;
    bool gap = false;
    for(int i = rpos.size() - 1; i >= 0; i--) {
        Node *n = rpos[i];
        if (n->isCFG() || n->isMultiHead()) {
            if (!gap) builder << "\n";
            n->printLine(builder);
            while(--i >= 0) {
                Node *m = rpos[i];
                if (!m->isMultiTail()) {i++; break;}
                m->printLine(builder);
            }
            builder << "\n";
            gap = true;
        } else {
            n->printLine(builder);
            gap = false;
        }
    }
    return builder.str();
}

// This requires recursion
void IRPrinter::postOrd(Node *n, Tomi::Vector<Node *> &rpos, Tomi::BitArray<10> &visit, Tomi::BitArray<10> &bfs) {
    if (!bfs.test(n->nid)) return; // NOT in the BFS visit
    if (visit.test(n->nid)) return; // Already post order walked
    visit.set(n->nid);
    if (n->isCFG()) {
        for (Node *use: n->outputs) {
            if (use != nullptr && use->isCFG() && use->nOuts() >= 1 && !(dynamic_cast<LoopNode *>(use->outputs[0]))) {
                postOrd(use, rpos, visit, bfs);
            }
        }
        for (Node *use: n->outputs) {
            if (use != nullptr && use->isCFG()) postOrd(use, rpos, visit, bfs);
        }
    }
    for (Node *use: n->outputs) {
        if (use != nullptr) postOrd(use, rpos, visit, bfs);
    }
    // Post order
    rpos.push_back(n);
}

BFS::BFS(Node *root, int d) {
    depth_ = d;
    add(root);
    int idx = 0;
    int lim = 1;
    while (idx < _bfs.size()) {
        Node *n = _bfs[idx++];
        for (auto def: n->inputs) {
            if (def != nullptr && !_bs.test(def->nid)) {
                add(def);
            }
        }
        if (idx == lim) {
            if (--d < 0) break;
            lim = _bfs.size();
        }

    }
    // Toss things past the limit except multi-heads
    while (idx < _bfs.size()) {
        Node *n = _bfs[idx++];
        if (n->isMultiHead()) idx++;
        else del(idx);
    }
    // Root set is any node with no inputs in the visited set
    lim = _bfs.size();
    for (int i = _bfs.size() - 1; i >= 0; i--) {
        if (!any_visited(_bfs[i])) {
            swap(i, --lim);
        }
    }
    lim_ = lim;
}

void BFS::swap(int x, int y) {
    if (x == y) return;
    Node *tmp = _bfs[x];
    Node *tx = _bfs[x];
    Node *ty = _bfs[y];
    _bfs[x] = ty;
    _bfs[y] = tx;
}

void BFS::add(Node *n) {
    _bfs.push_back(n);
    _bs.set(n->nid);
}

void BFS::del(int idx) {
    _bfs.erase(std::next(_bfs.begin(), idx));
    _bs.reset(idx);
}

bool BFS::any_visited(Node *n) {
    for (auto def: n->inputs) {
        if (def != nullptr && _bs.test(def->nid)) return true;
    }
    return false;
}