#ifndef LOAD_NODE_H
#define LOAD_NODE_H
/**
 * Load represents extracting a value from inside a memory object,
 * in chapter 10 this means Struct fields.
 */
#include "../../Include/type/type.h"
#include "../../Include/node/mem_op_node.h"
#include "../../Include/node/phi_node.h"

class LoadNode: public MemOpNode {
public:
    // Load a value from a ptr.field.
    LoadNode(std::string name, int alias, Type* glb, Node* mem, Node* ptr, Node*off);

    std::string label() override;
    std::string glabel() override;
    std::ostringstream &print_1(std::ostringstream &builder, Tomi::Vector<bool>& visited) override;

    Type* compute() override;
    Node* idealize() override;

    // Profitable if we find a matching Store on this Phi arm.
    bool profit(PhiNode* phi, int idx);
    Node*ld(int idx);
    static bool neverAlias(Node*ptr1, Node*ptr2);
};
#endif