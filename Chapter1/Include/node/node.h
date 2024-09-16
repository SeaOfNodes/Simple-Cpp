#ifndef NODE_H
#define NODE_H

#include <algorithm>
#include <iostream>
#include <vector>

#include <sstream>

/**
 * All Nodes in the Sea of Nodes IR inherit from the Node class.
 * The Node class provides common functionality used by all subtypes.
 * Subtypes of Node specialize by overriding methods.
 */

class Node {
public:
  int nid;
  std::vector<Node *> inputs;
  std::vector<Node *> outputs;

private:
  static int UNIQUE_ID;

public:
  Node() = default;
  Node(std::initializer_list<Node *> inputNodes);
  virtual ~Node() = default;
  [[nodiscard]] Node *in(std::size_t i) const;
  [[nodiscard]] std::size_t nIns() const;
  [[nodiscard]] std::size_t nOuts() const;

  [[nodiscard]] bool isUnused() const;
  [[nodiscard]] virtual bool isCFG() const;

  virtual std::string label();
  virtual std::string glabel();

  static void reset();
  // Debugger Printing

  std::string uniqueName();
};

#endif