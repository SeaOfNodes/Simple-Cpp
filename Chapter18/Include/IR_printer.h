#ifndef IR_PRINTER_H
#define IR_PRINTER_H

#include <iostream>
#include "../Include/tomi.h"

// forward declare Node

class CFGNode;

class Node;

class IRPrinter {
public:
    static std::string prettyPrint(Node *node, int depth);

    static std::string prettyPrintScheduled(Node *node, int depth);

    static std::string label(CFGNode *blk);

    static void label(std::ostringstream &sb, CFGNode *blk);

    static std::ostringstream& printLine(Node *n, std::ostringstream &sb);

    static void printLine_(Node *n, std::ostringstream &sb);

    static void
    printLine(Node *n, std::ostringstream &sb, Tomi::Vector<Node *> &bns, int i, Tomi::HashMap<Node *, int> &ds);

private:
    static void postOrd(Node *n, Node*prior, Tomi::Vector<Node *> &rpos, Tomi::BitArray<10> &visit, Tomi::BitArray<10> &bfs);

    static void walk_(Tomi::HashMap<Node *, int> &visit, Node *n, int d);
};

// Breadth-first search, broken out in a class to keep in more independent.
// Maintains a root-set of Nodes at the limit (or past by 1 if MultiHead).
class BFS {
public :
    Tomi::Vector<Node *> _bfs;
    Tomi::BitArray<10> _bs; // Visited members by node id
    int depth_{}; // Depth limit
    int lim_{}; // From here to _bfs._len can be roots for a reverse search
    BFS(Node *root, int d);

    void swap(int x, int y);

    void add(Node *n);

    void del(int idx);

    bool any_visited(Node *n);
};

#endif