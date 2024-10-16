#ifndef NODE_H
#define NODE_H

#include "../../Include/type/type.h"
#include <algorithm>
#include <cassert>
#include <iostream>
#include <sstream>
#include <vector>

/**
 * All Nodes in the Sea of Nodes IR inherit from the Node class.
 * The Node class provides common functionality used by all subtypes.
 * Subtypes of Node specialize by overriding methods.
 */

template <typename T>
typename std::vector<T>::Iterator find(std::vector<T> &vec, const T &value);

template <typename T>
void del(std::vector<T> &vec, typename std::vector<T>::iterator::pos);

class Node {
public:
  int nid;
  std::vector<Node *> inputs;
  std::vector<Node *> outputs;

  /**
   * The top of this stack represents current scope.
   *
   */

  Type *type_;

  /**
     * Immediate dominator tree depth, used to approximate a real IDOM during
     * parsing where we do not have the whole program, and also peepholes
     * change the CFG incrementally.
     * <p>
     * See {@link <a href="https://en.wikipedia.org/wiki/Dominator_(graph_theory)">...</a>}
   */
   int i_depth{};

private:
  static int UNIQUE_ID;

  Node *deadCodeElim(Node *m);

public:
  Node() = default;

  Node(std::initializer_list<Node *> inputNodes);
  Node(std::vector<Node *> inputs);

  virtual ~Node() = default;

  [[nodiscard]] Node *in(std::size_t i) const;

  [[nodiscard]] std::size_t nIns() const;

  [[nodiscard]] std::size_t nOuts() const;

  [[nodiscard]] bool isUnused() const;

  [[nodiscard]] virtual bool isCFG();

  virtual std::string label() = 0;

  virtual Node *copy(Node *lhs, Node *rhs);

  virtual std::string glabel();

  Node *keep();

  Node *unkeep();
  // Replace self with nnn in the graph, making 'this' go dead
  void subsume(Node* nnn);

  // This is a *deep* print.  This version will fail on cycles, which we will
  // correct later when we can parse programs with loops.  We print with a
  // tik-tok style; the common _print0 calls the per-Node _print1, which
  // calls back to _print0;
  std::ostringstream &print();

  // This is the common print: check for DEAD and print "DEAD" else call the
  // per-Node print1.
  virtual std::ostringstream &print_0(std::ostringstream &builder);

  static void reset();

  // Every Node implements this.
  virtual std::ostringstream &print_1(std::ostringstream &builder) = 0;

  virtual std::string uniqueName();

  Node *setDef(int idx, Node *new_def);

  virtual Type *compute();

  virtual Node *idealize();

  virtual bool isMultiHead();
  virtual bool isMultiTail();

  void popN(std::size_t n);
  // does this node contain all constants
  // Ignores i(0), as is usually control.
  bool allCons();

  // Return the immediate dominator of this Node and compute dom tree depth.
  virtual Node* idom();

  // Remove the numbered input, compressing the inputs in-place. This
  // shuffles the order deterministically - which is suitable for Region and
  // Phi, but not for every Node.
  void delDef(int idx);
  Node *peephole();

  /*
   * Find a node by index.
   * */

  Node *find(std::vector<bool> visit, int nid_);

  Node *addDef(Node *new_def);

  void kill();

  bool isDead();

  static bool disablePeephole;

protected:
  Node *addUse(Node *n);

  bool delUse(Node *use);

public:
  // Peephole utilities
  Node *swap12();
};

#endif
