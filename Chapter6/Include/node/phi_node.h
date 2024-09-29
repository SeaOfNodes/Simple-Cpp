#ifndef PHI_NODE_H
#define PHI_NODE_H
#include "../../Include/node/node.h"

#include <initializer_list>

class PhiNode : public Node {
public:
  std::string label_;
  PhiNode(std::string label, std::initializer_list<Node *> inputs);
  PhiNode(std::string label, std::vector<Node *> inputs);

  std::string label() override;
  std::string glabel() override;

  std::ostringstream &print_1(std::ostringstream &builder) override;

  Node *region();

  Type *compute() override;
  Node *idealize() override;

private:
  bool same_op();
  bool same_inputs();
};
#endif