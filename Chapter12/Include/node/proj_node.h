#ifndef PROJ_NODE_H
#define PROJ_NODE_H
#include "../../Include/node/node.h"
#include "../../Include/node/multi_node.h"
#include "../../Include/type/tuple_type.h"
#include "../../Include/node/if_node.h"
#include "../type/tuple_type.h"


class ProjNode : public Node {
public:
  const int idx_{};
  std::string label_;
  ProjNode(Node* ctrl, int idx, std::string label);
  std::string label() override;
  std::ostringstream& print_1(std::ostringstream& builder, Tomi::Vector<bool>& visited) override;
  bool isCFG() override;

  MultiNode* ctrl();

  bool isPinned();
  bool isMultiTail();

  bool isMem() override;

  Type* compute() override;
  Node* idealize() override;
  bool eq(Node*n) override;
  int hash() override;
};
#endif