#ifndef NOT_NODE_H
#define NOT_NODE_H
#include "../../Include/node/node.h"

class NotNode : public Node {
public:
  NotNode(Node* in);
  std::string label() override;
  std::string glabel() override;

  std::ostringstream &print_1(std::ostringstream &builder) override;
  Type *compute() override;

  Node *idealize();
};
#endif