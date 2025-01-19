#ifndef LIST_SCHEDULER_H
#define LIST_SCHEDULER_H
#include "../Include/tomi.h"
#include "../Include/codegen.h"

class ListScheduler {
public:
    Tomi::BitArray<10> VISIT;
    static Tomi::Vector<int> BCNTS;
    static void sched(CodeGen*code);

    static void walk(CFGNode* cfg, Tomi::BitArray<10>& visit);

    static void local(CFGNode* bb);

    static int ready(CFGNode* bb, Node*use, int ready);

    // Pick best between sched and ready.
    static int best(Tomi::Vector<Node*> uses, int sched, int ready);

    // Highest score wins.  Max at 1000, min at 10, except specials.
    static int score(Node*n);

}

#endif