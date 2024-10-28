#ifndef PHI_NODE_H
#define PHI_NODE_H
#include "../../Include/node/node.h"

#include "../../Include/node/region_node.h"

#include <initializer_list>

class PhiNode : public Node {
public:
  std::string label_;
  PhiNode(std::string label, std::initializer_list<Node *> inputs);
  PhiNode(std::string label, std::vector<Node *> inputs);

  std::string label() override;
  std::string glabel() override;

  std::ostringstream &print_1(std::ostringstream &builder, std::vector<bool>& visited) override;
  bool isMultiTail() override;

  Node *region();

  Type *compute() override;
  Node *idealize() override;

  Node *singleUniqueInput();

  bool allCons() override;
private:
  bool same_op();
};
#endif