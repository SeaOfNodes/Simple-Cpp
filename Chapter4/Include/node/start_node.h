#ifndef START_NODE_H
#define START_NODE_H

#include "../../Include/node/node.h"
#include "../../Include/type/integer_type.h"
#include "../../Include/type/tuple_type.h"
class StartNode : public Node {
public:
  TypeTuple* args_;
  StartNode(std::initializer_list<Type*> args);
  bool isCFG() const override;
  std::ostringstream &print_1(std::ostringstream &builder);

  Type *compute() override;
  Node *idealize() override;
  std::string label() override;
};

#endif