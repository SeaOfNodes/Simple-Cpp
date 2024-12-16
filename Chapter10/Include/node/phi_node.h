#ifndef PHI_NODE_H
#define PHI_NODE_H
#include "../../Include/node/node.h"

#include "../../Include/node/region_node.h"

#include <initializer_list>

class PhiNode : public Node {
public:
  std::string label_;
  Type* declaredType = nullptr;
  PhiNode(std::string label, Type* declaredType, std::initializer_list<Node *> inputs);
  PhiNode(std::string label, Type* declaredType, Tomi::Vector<Node *> inputs);

  std::string label() override;
  std::string glabel() override;

  std::ostringstream &print_1(std::ostringstream &builder, Tomi::Vector<bool>& visited) override;
  bool isMultiTail() override;

  Node *region();

  Type *compute() override;
  Node *idealize() override;
 bool isMem() override;

  Node *singleUniqueInput();

  bool inProgress();

  bool allCons(Node*) override;
private:
  bool same_op();
  bool eq(Node* n) override;
};
#endif