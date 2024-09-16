#ifndef ADD_NODE_H
#define ADD_NODE_H
#include "../Include/node/node.h"
#include <iostream>
#include <memory>


class AddNode : public Node {
public:
  /*
   * First input is usually a control Node.
   * Node({nullptr, lhs, rhs})
   * */
  AddNode(Node* lhs, Node* rhs);
  std::string label() override;
  std::string glabel() override;
  std::ostringstream& print_1(std::ostringstream& builder) override;
  /*
   * Computes a new type from the type_ field of its inputs.
   * */
  Type* compute() override;
  Node* idealize() override;
};

#endif