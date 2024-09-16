#ifndef NODE_H
#define NODE_H

#include <vector>
#include <algorithm>
#include <iostream>
#include "../../Include/type/type.h"
#include <cassert>
#include<sstream>

/**
* All Nodes in the Sea of Nodes IR inherit from the Node class.
* The Node class provides common functionality used by all subtypes.
* Subtypes of Node specialize by overriding methods.
 */

template <typename T>
typename std::vector<T>::Iterator find(std::vector<T>& vec, const T& value);

template <typename T>
void del(std::vector<T>& vec, typename std::vector<T>::iterator::pos);

class Node {
public:
  int nid;
  std::vector<Node*> inputs;
  std::vector<Node*> outputs;

  /**
   * The top of this stack represents current scope.
   *
   */

  Type* type_;

private:
  static int UNIQUE_ID;
public:
    Node() = default;
    Node(std::initializer_list<Node*> inputNodes);
    virtual ~Node() = default;
    [[nodiscard ]] Node* in(std::size_t i) const;
    [[nodiscard]] std::size_t nIns() const;
    [[nodiscard]] std::size_t nOuts() const;

    [[nodiscard]]bool isUnused() const;
    [[nodiscard]]virtual bool isCFG() const;

    virtual std::string label() = 0;
    virtual std::string glabel();

    // This is a *deep* print.  This version will fail on cycles, which we will
    // correct later when we can parse programs with loops.  We print with a
    // tik-tok style; the common _print0 calls the per-Node _print1, which
    // calls back to _print0;
    std::ostringstream& print();
    // This is the common print: check for DEAD and print "DEAD" else call the
    // per-Node print1.
    virtual std::ostringstream& print_0(std::ostringstream& builder);
    static void reset();
    // Every Node implements this.
    virtual std::ostringstream& print_1(std::ostringstream& builder) = 0;
    virtual std::string uniqueName();
    Node* setDef(int idx, Node* new_def);

    virtual Type* compute();
    virtual Node* idealize();

    void popN(int n);

    Node* peephole();
    /*
     * Find a node by index.
     * */

    Node* find(std::vector<bool> visit, int nid_);
    Node* addDef(Node* new_def);

    void kill();
    bool isDead();

    static bool disablePeephole;
protected:
  Node* addUse(Node* n);
  bool delUse(Node* use);
};

#endif