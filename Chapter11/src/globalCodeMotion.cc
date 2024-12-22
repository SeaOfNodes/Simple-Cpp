#include "../Include/globalCodeMotion.h"
#include "../Include/parser.h"
#include "../Include/node/store_node.h"
#include <cassert>
#include <algorithm>

void GlobalCodeMotion::buildCFG(StopNode *stop) {
    fixLoops(stop);
    schedEarly();
    Parser::SCHEDULED = true;
    schedLate(Parser::START);
}

void GlobalCodeMotion::fixLoops(StopNode *stop) {
    Tomi::BitArray<10> visited;
    Tomi::HashSet<CFGNode *> unreach;
    unreach.put(Parser::START);
    for (Node *ret: stop->inputs) {
        ((ReturnNode *) ret)->walkUnreach(visited, unreach);
    }
    if (unreach.isEmpty()) return;
    // Forwards walk from unreachable, looking for loops with no exit test.
    visited.reset();
    for (auto cfg: unreach) {
        walkInfinite(cfg.getValue(), visited, stop);
    }
    // Set loop depth on remaining graph
    unreach.clear();
    visited.reset();
    for (Node *ret: stop->inputs) {
        ((ReturnNode *) ret)->walkUnreach(visited, unreach);
    }
    assert(unreach.isEmpty());
}

void GlobalCodeMotion::walkInfinite(CFGNode *n, Tomi::BitArray<10> &visited, StopNode *stop) {
    if (visited.test(n->nid)) return;
    visited.set(n->nid);
    if (auto *loop = dynamic_cast<LoopNode *>(n)) {
        loop->forceExit(stop);
    }
    for (Node *use: n->outputs) {
        if (dynamic_cast<CFGNode *>(use) != nullptr) {
            walkInfinite((CFGNode *) use, visited, stop);
        }
    }
}

void GlobalCodeMotion::schedEarly() {
    Tomi::Vector<CFGNode *> rpo;
    Tomi::BitArray<10> visit;
    rpo_cfg(Parser::START, visit, rpo);
    // Reverse Post-Order on CFG
    for (int j = rpo.size() - 1; j >= 0; j--) {
        CFGNode *cfg = rpo[j];
        cfg->loopDepth();
        for (Node *n: cfg->inputs) {
            schedEarly_(n, visit);
            // Strictly for dead infinite loops, we can have entire code blocks
            // not reachable from below - so we reach down, from above, one
            // step.  Since _schedEarly modifies the output arrays, the normal
            // region._outputs ArrayList iterator throws CME.  The extra edges
            // are always *added* after any Phis, so just walk the Phi prefix.
        }
        if (auto *region = dynamic_cast<RegionNode *>(cfg)) {
            std::size_t len = region->nOuts();
            for (std::size_t i = 0; i < len; i++) {
                if (auto *phi = dynamic_cast<PhiNode *>(region->out(i))) {
                    schedEarly_(phi, visit);
                }
            }
        }
    }
}

void GlobalCodeMotion::rpo_cfg(Node *n, Tomi::BitArray<10> &visited, Tomi::Vector<CFGNode *> &rpo) {
    auto *cfg = dynamic_cast<CFGNode *>(n);
    if (!cfg && visited.test(cfg->nid)) {
        return;
    }
    visited.set(cfg->nid);
    for (Node *use: cfg->outputs) {
        rpo_cfg(use, visited, rpo);
    }
    rpo.push_back(cfg);

}

void GlobalCodeMotion::schedEarly_(Node *n, Tomi::BitArray<10> &visit) {
    if (n == nullptr || visit.test(n->nid)) return; // Been there, done that
    visit.set(n->nid);
    // Schedule not-pinned not-CFG inputs before self.  Since skipping
    // Pinned, this never walks the backedge of Phis (and thus spins around
    // a data-only loop, eventually attempting relying on some pre-visited-
    // not-post-visited data op with no scheduled control.
    for (Node *def: n->inputs) {
        if (def != nullptr && !def->isPinned()) {
            schedEarly_(def, visit);
        }
    }
    // If not-pinned (e.g. constants, projections, phi) and not-CFG
    if (!n->isPinned()) {
        // Schedule at deepest input
        CFGNode *early = Parser::START;
        for (int i = 1; i < n->nIns(); i++) {
            if (n->in(i)->cfg0()->idepth() > early->idepth()) {
                early = n->in(i)->cfg0();
            }
        }
        n->setDef(0, early);
    }
}

void GlobalCodeMotion::schedLate(StartNode *start) {
    Tomi::Vector<CFGNode *> late(Node::UID());
    Tomi::Vector<Node *> ns(Node::UID());

    schedLate_(start, ns, late);
    for (int i = 0; i < late.size(); i++) {
        if (ns[i] != nullptr) {
            ns[i]->setDef(0, late[i]);
        }
    }
}

void GlobalCodeMotion::schedLate_(Node *n, Tomi::Vector<Node *> &ns, Tomi::Vector<CFGNode *> &late) {
    if (late[n->nid] != nullptr) return; // been there done that
    // These I know the late schedule of, and need to set early for loops
    if (auto *cfg = dynamic_cast<CFGNode *>(n)) late[n->nid] = cfg->blockHead() ? cfg : cfg->cfg(0);
    if (auto *phi = dynamic_cast<PhiNode *>(n)) late[n->nid] = phi->region();

    // Walk Stores before Loads, so we can get the anti-deps right
    for (Node *use: n->outputs) {
        if (isForwardsEdge(use, n) && dynamic_cast<TypeMem *>(use->type_)) schedLate_(use, ns, late);

    }
    // Walk everybody now
    for (Node *use: n->outputs) {
        if (isForwardsEdge(use, n)) schedLate_(use, ns, late);
    }
    // Already implicitly scheduled
    if (n->isPinned()) return;
    // Need to schedule n
    // Walk uses, gathering the LCA (Least Common Ancestor) of uses
    CFGNode *early = (CFGNode *) n->in(0);
    assert(early != nullptr);
    CFGNode *lca = nullptr;
    for (Node *use: n->outputs) {
        lca = use_block(n, use, late)->idom(lca);
    }
    // Loads may need anti-dependencies, raising their LCA
    if (auto *load = dynamic_cast<LoadNode *>(n)) {
        lca = find_anti_dep(lca, load, early, late);
    }
    // Walk up from the LCA to the early, looking for best place.  This is the
    // lowest execution frequency, approximated by least loop depth and
    // deepest control flow.
    CFGNode *best = lca;
    lca = lca->idom();
    for (; lca != early->idom(); lca = lca->idom()) {
        if (better(lca, best)) best = lca;
    }
    assert(!dynamic_cast<IfNode *>(best));
    ns[n->nid] = n;
    late[n->nid] = best;
}

CFGNode *GlobalCodeMotion::use_block(Node *n, Node *use, Tomi::Vector<CFGNode *> &late) {
    auto *phi = dynamic_cast<PhiNode *>(use);
    if (!phi) return late[use->nid];
    CFGNode *found = nullptr;
    for (int i = 1; i < phi->nIns(); i++) {
        if (phi->in(i) == n) {
            if (found == nullptr) found = phi->region()->cfg(i);
        }
    }
    assert(found != nullptr);
    return found;
}

bool GlobalCodeMotion::better(CFGNode *lca, CFGNode *best) {
    return lca->loopDepth_ < best->loopDepth_ ||
           (lca->idepth() > best->idepth() || dynamic_cast<IfNode *>(best));
}

bool GlobalCodeMotion::isForwardsEdge(Node *use, Node *def) {
    return use != nullptr && def != nullptr && !(use->nIns() > 2 && use->in(2) == def &&
                                                 (dynamic_cast<LoopNode *>(use) || (dynamic_cast<PhiNode *>(use) &&
                                                                                    dynamic_cast<LoopNode *>(dynamic_cast<PhiNode *>(use)->region()))));
}

CFGNode *GlobalCodeMotion::find_anti_dep(CFGNode *lca, LoadNode *load, CFGNode *early, Tomi::Vector<CFGNode *> late) {
    // We could skip final-field loads here.
    // Walk LCA->early, flagging Load's block location choices
    for (CFGNode *cfg = lca; early != nullptr && cfg != early->idom(); cfg = cfg->idom()) {
        cfg->anti_ = load->nid;
    }

    // Walk load->mem uses, looking for Stores causing an anti-dep
    for (Node *mem: load->mem()->outputs) {
        if (auto *st = dynamic_cast<StoreNode *>(mem)) {
            lca = anti_dep(load, late[st->nid], st->cfg0(), lca, st);
        } else if (auto *phi = dynamic_cast<PhiNode *>(mem)) {
            // Repeat anti-dep for matching Phi inputs.
            // No anti-dep edges but may raise the LCA.
            for (int i = 1; i < phi->nIns(); i++) {
                if (phi->in(i) == load->mem()) {
                    lca = anti_dep(load, phi->region()->cfg(i), load->mem()->cfg0(), lca, nullptr);
                }
            }
        } else if (dynamic_cast<LoadNode *>(mem)) {
            // Loads do not cause anti-deps on other loads
            continue;
        } else if (dynamic_cast<ReturnNode *>(mem)) {
            // Load must already be ahead of Return
            continue;
        } else {
            throw std::runtime_error("TODO");
        }
    }

    return lca;
}

CFGNode *GlobalCodeMotion::anti_dep(LoadNode *load, CFGNode *stblk, CFGNode *defblk, CFGNode *lca, Node *st) {
    // Walk store blocks "reach" from its scheduled location to its earliest
    for (; stblk != defblk->idom(); stblk = stblk->idom()) {
        // Store and Load overlap, need anti-dependence
        if (stblk->anti_ == load->nid) {
            lca = lca->idom(stblk);
            if (lca == stblk && st != nullptr &&
                std::find(st->inputs.begin(), st->inputs.end(), load) == st->inputs.end()) {
                st->addDef(load);
            }
            return lca;
        }
    }
    return lca;
}