#ifndef START_NODE_H
#define START_NODE_H

#include "../../Include/type/integer_type.h"
#include "../../Include/type/tuple_type.h"
#include "../../Include/type/type_struct.h"
#include "../../Include/type/type_mem.h"

#include "multi_node.h"
#include "scope_node.h"
#include "node.h"


class StartNode : public MultiNode {
public:
  Type *arg_;
  StartNode(Type*arg);

  bool isCFG() override;

  std::ostringstream &print_1(std::ostringstream &builder, Tomi::Vector<bool>& visited) override;

  Type *compute() override;

  CFGNode* cfg0();
  Node *idealize() override;
  CFGNode *idom(Node* dep) override;

    /**
   * Creates a projection for each of the struct's fields, using the field alias
   * as the key.
   */
   void addMemProj(TypeStruct* ts, ScopeNode* scope);

  bool isMultiHead() override;

  virtual void walkUnreach_(Tomi::BitArray<10> &vitisted, Tomi::HashSet<CFGNode *>& unreach) override;
  int loopDepth() override;
  int idepth() override;
  bool blockHead() override;
  std::string label() override;
};

#endif
