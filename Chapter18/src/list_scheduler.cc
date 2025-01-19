#include "../Include/list_scheduler.h"
#include "../Include/node/proj_node.h"
#include "../Include/node/return_node.h"

void ListScheduler::sched(CodeGen *code) {
    BCNTS = Tomi::Vector<int>(Node::UID());
    walk(code->start_, VISIT);
    VISIT.clear();
}

void ListScheduler::walk(CFGNode *cfg, Tomi::BitArray<10> &visit) {
    if(visit[cfg->id_]) {
        return;
    }
    visit.set(cfg->id_);
    local(cfg);

    for(Node*n: cfg->outputs) {
        if(dynamic_cast<CFGNode*>(n)) {
            walk(c, visit);
        }
    }
}

// Local schedule this one CFG
static void ListScheduler::local(CFGNode *bb) {
    if(!bb->blockHead() || dynamic_cast<StopNode*>(bb)) return;
    Tomi::Vector<Node*> uses = bb->outputs;
    int ulen = bb0->outputs.size();

    // Count block-locals
    for(int i = 0; i < ulen; i++) {
        Node*use = uses[i];
        if(use->in(0) == bb && !(dynamic_cast<PhiNode*>(use))) {
            // Normal basic block member: in(0)==bb and not a Phi
            for(Node*def: use->inputs) {
                // Has defs from same block?  Then block-local inputs
                if(def != nullptr && def->cfg0() == bb) BCNTS[use->nid]++;
            }
        }
    }
    // Nodes are ready if they are not used by other nodes in this block.
    // Move ready up front. We will never look at remaining nodes, later
    // nodes are found by looking at users of ready nodes.
    int ready = 0;
    for(int i = 0; i < ulen; i++) {
        if(BCNTS[uses[i]->nid] == 0) { // No block-local inputs?
           uses[ready++] = uses[i]; // Move into ready set
        }
    }
    // Classic list scheduler.  Behind sched is scheduled.  Between sched
    // and ready have zero counts and are ready to schedule.  Ahead of
    // ready is undefined.
    //
    // As nodes are scheduled, the nodes they use decrement their _bcnt. Once
    // that gets to 0, all the nodes using it are scheduled, so it's ready.
    int sched = 0;
    while(sched < ulen) {
        int pick = best(uses, sched, ready); // Pick best
        Node*best = uses[pick];
        uses[pick] = uses[sched]; // Swap to front
        uses[sched++] = best; // And move it into the scheduled set
        // Lower ready count of users
        for(Node*use: best->outputs) {
            if(dynamic_cast<ProjNode*>(use)) {
                for(Node* useuse: use->outputs) {
                    ready = ready(bb, useuse, ready);
                }
            } else {
                ready = ready(bb, use, ready);
            }
        }
    }
}

int ListScheduler::ready(CFGNode *bb, Node *use, int ready) {
    if(use != nullptr && use->in(0) == bb && !(dynamic_cast<PhiNode*>(use))) {
        if(--BCNTS[use->nid] == 0) bb->outputs[ready++] = use; // Became ready, move into ready set
    }
    return ready;
}

int ListScheduler::best(Tomi::Vector<Node*> uses, int sched, int ready) {
    int pick = sched;
    int score = score(uses[pick]);
    for(int i = sched + 1; i < ready; i++) {
        int nscore = score(uses[i]);
        if(nscore > score) {
            score = nscore;
            pick = i;
        }
    }
    return pick;
}

int ListScheduler::score(Node*n) {
    if(n->isMultiTail()) return 1001;  // Pinned just behind the multi-head
    if(dynamic_cast<CFGNode*>(n)) return dynamic_cast<ReturnNode*>(n) ? 2 : 1; Pinned at block exit

    // Nothing special
    return 500;
}