#include "../Include/IR_printer.h"
#include "../Include/node/node.h"
#include "../Include/node/region_node.h"
#include "../Include/parser.h"
#include "../Include/node/cfg_node.h"
#include "../Include/utils.h"
#include <limits>

void IRPrinter::printLine(Node *n, std::ostringstream &sb) {
    printLine_(n, sb);

}

void IRPrinter::printLine(Node *n, std::ostringstream &sb, Tomi::Vector<Node *> &bns, int i,
                          Tomi::HashMap<Node *, int> &ds) {
    printLine(n, sb);
    Utils::del(bns, i);
    ds.remove(n);
}

void IRPrinter::printLine_(Node *n, std::ostringstream &builder) {
    builder << std::format("{:>4} {:<7.7}", n->nid, n->label());
    if(n->inputs.empty()) {
        builder << "DEAD\n";
        return;
    }

    for (Node *def: n->inputs) {
        if (def == nullptr) builder << "____";
        else builder << std::format("{:>4} ", def->nid);;
    }
    builder << std::string(4 * 5, ' ').c_str();

    builder << " [[  ";
    for (Node *use: n->outputs) {
        if (use == nullptr) builder << "____";
        else builder << std::format("{:>4} ", use->nid);
    }
    int lim = 5 - std::max(static_cast<int>(n->inputs.size()), 3);
    for (size_t i = n->outputs.size(); i < lim; i++) {
        builder << "      ";
    }
    builder << " ]] ";
    if (n->type_ != nullptr) n->type_->print_1(builder);
    builder << "\n";
}

// Another bulk pretty-printer.  Makes more effort at basic-block grouping.
std::string IRPrinter::prettyPrint(Node *node, int depth) {
    if (Parser::SCHEDULED) {
        return prettyPrintScheduled(node, depth, false);
    }
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
    for (int i = rpos.size() - 1; i >= 0; i--) {
        Node *n = rpos[i];
        if (n->isCFG() || n->isMultiHead()) {
            if (!gap) builder << "\n";
            printLine(n, builder);
            while (--i >= 0) {
                Node *m = rpos[i];
                if (!m->isMultiTail()) {
                    i++;
                    break;
                }
                printLine(n, builder);
            }
            builder << "\n";
            gap = true;
        } else {
            printLine(n, builder);
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

std::string IRPrinter::prettyPrintScheduled(Node *node, int depth, bool llvmFormat) {
    Tomi::HashMap<Node *, int> ds;
    walk_(ds, node, depth);
    std::ostringstream builder;
    Tomi::Vector<Node *> bns;
    int counter{}; // debugging purposes
    while (!ds.isEmpty()) {
        CFGNode *blk = nullptr;
        for (auto n: ds) {
            auto *cfg0 = dynamic_cast<CFGNode *>(n.key);
            auto *cfg = cfg0 && cfg0->blockHead() ? cfg0 : dynamic_cast<CFGNode *>(n.key->in(0));
            if (blk == nullptr || cfg->idepth() < blk->idepth()) {
                blk = cfg;
            }
        }
        ds.remove(blk);
        builder << label(blk) << ":";
        builder << std::string(4 * 5, ' ').c_str();
        builder << "[[ ";
        if (dynamic_cast<RegionNode *>(blk) || dynamic_cast<StopNode *>(blk)) {
            auto *ifa = dynamic_cast<StopNode *>(blk);
            for (int i = (ifa ? 0 : 1); i < blk->nIns(); i++) {
                label(builder, blk->cfg(i));
            }
        } else if (!dynamic_cast<StartNode *>(blk)) {
            label(builder, blk->cfg(0));
        }
        builder << "]]\n";
        // Collect block contents that are in the depth limit
        bns.clear();
        int xd = std::numeric_limits<int>::max();
        for (Node *use: blk->outputs) {
            int *i = ds.get(use);
            auto *cfg = dynamic_cast<CFGNode *>(use);
            if (i != nullptr && !(cfg && cfg->blockHead())) {
                bns.push_back(use);
                xd = std::min(xd, *i);
            }
        }
        // Print Phis up front, if any
        for (int i = 0; i < bns.size(); i++) {
            auto *phi = dynamic_cast<PhiNode *>(bns[i]);
            if (phi) {
                printLine(phi, builder, bns, i--, ds);
            }
        }
        // Print block contents in depth order, bumping depth until whole block printed
        for (; !bns.empty(); xd++) {
            for (int i = 0; i < bns.size(); i++) {
                if (*ds.get(bns[i]) == xd) {
                    auto current = bns[i];
                    printLine(current, builder, bns, i--, ds);
                }
            }
        }
        builder << "\n";
        counter++;
    }
    return builder.str();
}

void IRPrinter::label(std::ostringstream &sb, CFGNode *blk) {
    if (!blk->blockHead()) blk = blk->cfg(0);
    sb << label(blk) << " ";

}

std::string IRPrinter::label(CFGNode *blk) {
    if (dynamic_cast<StartNode *>(blk)) return "START";
    if (dynamic_cast<LoopNode *>(blk)) return "LOOP" + std::to_string(blk->nid);
    else return "L" + std::to_string(blk->nid);
}

void IRPrinter::walk_(Tomi::HashMap<Node *, int> &ds, Node *node, int d) {
    int *nd = ds.get(node);
    if (nd != nullptr && d <= *nd) return;
    ds.put(node, d);
    if (d == 0) return;
    for (Node *def: node->inputs) {
        if (def != nullptr) walk_(ds, def, d - 1);
    }
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