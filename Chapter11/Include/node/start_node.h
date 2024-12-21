#ifndef START_NODE_H
#define START_NODE_H

#include "../../Include/type/integer_type.h"
#include "../../Include/type/tuple_type.h"
#include "../../Include/type/type_struct.h"
#include "../../Include/type/type_mem.h"

#include "multi_node.h"
#include "scope_node.h"
#include "node.h"


class StartNode : public MultiNode, public CFGNode {
public:
  TypeTuple *args_;
  static Tomi::HashMap<std::string, int> aliasStarts;
  StartNode(std::initializer_list<Type *> args);

  bool isCFG() override;

  std::ostringstream &print_1(std::ostringstream &builder, Tomi::Vector<bool>& visited) override;

  Type *compute() override;

  Node *idealize() override;
  CFGNode *idom() override;

    /**
   * Creates a projection for each of the struct's fields, using the field alias
   * as the key.
   */
   void addMemProj(TypeStruct* ts, ScopeNode* scope);

  bool isMultiHead() override;

  std::string label() override;
};

#endif
