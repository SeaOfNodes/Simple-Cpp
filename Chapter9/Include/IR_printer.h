#ifndef IR_PRINTER_H
#define IR_PRINTER_H

#include <iostream>
#include "../Include/tomi.h"


class Node;
// forward declare Node
class IRPrinter {
public:
    static std::string prettyPrint(Node* node, int depth);
private:
    static void postOrd(Node*n, Tomi::Vector<Node*>& rpos, Tomi::BitArray<10>& visit, Tomi::BitArray<10>& bfs);
};

// Breadth-first search, broken out in a class to keep in more independent.
// Maintains a root-set of Nodes at the limit (or past by 1 if MultiHead).
class BFS {
public :
    Tomi::Vector<Node*> _bfs;
    Tomi::BitArray<10> _bs; // Visited members by node id
    int depth_{}; // Depth limit
    int lim_{}; // From here to _bfs._len can be roots for a reverse search
    BFS(Node* root, int d);
    void swap(int x, int y);
    void add(Node* n);
    void del(int idx);
    bool any_visited(Node*n);
};
#endif