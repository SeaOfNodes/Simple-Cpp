#ifndef START_NODE_H
#define START_NODE_H

#include "../../Include/type/integer_type.h"
#include "../../Include/type/tuple_type.h"
#include "multi_node.h"
#include "node.h"

class StartNode : public MultiNode {
public:
  TypeTuple *args_;

  StartNode(std::initializer_list<Type *> args);

  bool isCFG() override;

  std::ostringstream &print_1(std::ostringstream &builder, Tomi::Vector<bool>& visited) override;

  Type *compute() override;

  Node *idealize() override;
  Node *idom() override;

  bool isMultiHead() override;

  std::string label() override;
};

#endif
