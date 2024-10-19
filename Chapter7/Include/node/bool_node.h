#include "../../Include/node/node.h"

#include "../../Include/node/add_node.h"
#include "../../Include/node/constant_node.h"
#include "../../Include/type/integer_type.h"

#ifndef BOOL_NODE_H
#define BOOL_NODE_H

class BoolNode : public Node {
public:
  virtual std::string op();

  [[nodiscard]] virtual bool doOp(long lhs, long rhs) const;

  BoolNode(Node *lhs, Node *rhs);

  std::string label() override;

  std::string glabel() override;

  std::ostringstream &print_1(std::ostringstream &builder,
                              std::vector<bool> visited) override;

  Type *compute() override;

  Node *idealize() override;
};

class EQ : public BoolNode {
public:
  EQ(Node *lhs, Node *rhs);

  Node *copy(Node *rhs, Node *lhs) override;

  std::string op() override;

  std::string label() override;

  bool doOp(long lhs, long rhs) const override;
};

class LT : public BoolNode {
public:
  LT(Node *lhs, Node *rhs);

  Node *copy(Node *rhs, Node *lhs) override;

  std::string label() override;

  std::string op() override;

  bool doOp(long lhs, long rhs) const override;
};

class LE : public BoolNode {
public:
  LE(Node *lhs, Node *rhs);

  Node *copy(Node *rhs, Node *lhs) override;

  std::string op() override;

  std::string label() override;

  bool doOp(long lhs, long rhs) const override;
};
#endif
