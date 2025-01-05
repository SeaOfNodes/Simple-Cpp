#ifndef ADD_NODE_H
#define ADD_NODE_H
#include "../../Include/node/phi_node.h"
#include "../../Include/type/integer_type.h"
#include "constant_node.h"
#include "mul_node.h"
#include "node.h"

#include <iostream>
#include <memory>

class AddNode : public Node {
public:
  /*
   * First input is usually a control Node.
   * Node({nullptr, lhs, rhs})
   * */
  AddNode(Node *lhs, Node *rhs);

  std::string label() override;

  std::string glabel() override;

  std::ostringstream &print_1(std::ostringstream &builder,
                              Tomi::Vector<bool>&) override;

  /*
   * Computes a new type from the type_ field of its inputs.
   * */
  Type *compute() override;

  static bool overflow(long x, long y);

  Node *idealize() override;

  Node *copy(Node *lhs, Node *rhs) override;
  Node* copyF() override;
  /*
   *
   * Rotation is only valid for associative ops, e.g. Add, Mul, And, Or.
   * Do we have ((phi cons)|(x + (phi cons)) + con|(phi cons)) ?
   * Push constant up through the phi: x + (phi con0+con0 con1+con1...)
   *
   * */
  static Node *phiCon(Node *op, bool rotate);
  static PhiNode *pcon(Node *op, Node* dep);

  static bool spine_cmp(Node *hi, Node *lo, Node* dep);
};

#endif
