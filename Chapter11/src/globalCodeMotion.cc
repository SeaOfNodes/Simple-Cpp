#include "../Include/globalCodeMotion.h"
#include "../Include/parser.h"
#include <cassert>

void GlobalCodeMotion::buildCFG(StopNode *stop) {
    fixLoops(stop);
    schedEarly();
    Parser::SCHEDULED = true;
    schedLate(Parser::START);
}

void GlobalCodeMotion::fixLoops(StopNode *stop) {
    Tomi::BitArray<10> visited;
    Tomi::HashSet<CFGNode*> unreach;
    unreach.put(Parser::START);
    for(Node* ret: stop->inputs) {
        ((ReturnNode*)ret)->walkUnreach(visited, unreach);
    }
    if(unreach.isEmpty()) return;
    // Forwards walk from unreachable, looking for loops with no exit test.
    visited.reset();
    for(auto cfg: unreach) {
        walkInfinite(cfg.getValue(), visited, stop);
    }
    // Set loop depth on remaining graph
    unreach.clear();
    visited.reset();
    for(Node* ret: stop->inputs) {
        ((ReturnNode*)ret)->walkUnreach(visited, unreach);
    }
    assert(unreach.isEmpty());
}

void GlobalCodeMotion::walkInfinite(CFGNode *n, Tomi::BitArray<10> &visited, StopNode *stop) {
    if(visited.test(n->nid)) return;
    visited.set(n->nid);
    if(auto* loop = dynamic_cast<LoopNode*>(n)) {
        loop->forceExit(stop);
    }
    for(Node* use: n->outputs) {
        if(dynamic_cast<CFGNode*>(use) != nullptr) {
            walkInfinite((CFGNode*)use, visited, stop);
        }
    }
}