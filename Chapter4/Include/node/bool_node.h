#include "../../Include/node/node.h"

#ifndef BOOL_NODE_H
#define BOOL_NODE_H

class BoolNode : public Node {
public:
  virtual std::string op();
  virtual bool doOp(long lhs, long rhs) const;

  BoolNode(Node *lhs, Node *rhs);
  std::string label() override;
  std::string glabel() override;
  std::ostringstream &print_1(std::ostringstream &builder) override;
  Type *compute() override;
  Node *idealize() override;
};

class EQ : public BoolNode {
public:
  EQ(Node *lhs, Node *rhs);
  std::string op() override;
  bool doOp(long lhs, long rhs) const override;
};

class LT : public BoolNode {
public:
  LT(Node *lhs, Node *rhs);
  std::string op() override;
  bool doOp(long lhs, long rhs) const override;
};

class LE : public BoolNode {
public:
  LE(Node *lhs, Node *rhs);
  std::string op() override;
  bool doOp(long lhs, long rhs) const override;
};
#endif