#ifndef NEW_NODE_H
#define NEW_NODE_H

#include "node.h"

#include "../../Include/node/multi_node.h"
#include "../type/type_mem_ptr.h"
/**
 *  Allocation!  Allocate a chunk of memory, and pre-zero it.
 *  The inputs include control and size, and ALL aliases being set.
 *  The output is large tuple, one for every alias plus the created pointer.
 *  New is expected to be followed by projections for every alias.
 */

class NewNode : public Node {
public:
    NewNode(TypeMemPtr* ptr, std::initializer_list<Node*> nodes);
    NewNode(TypeMemPtr* ptr, Tomi::Vector<Node*> nodes);
    TypeMemPtr* ptr_;
    int len_{};

    std::string label() override;
    std::string glabel() override;

    int find_alias(int alias);
    bool isPinned() override;
    std::ostringstream &print_1(std::ostringstream &builder, Tomi::Vector<bool>& visited) override;
    Type *compute() override;
    Node *idealize() override;
    bool eq(Node* n) override;
    int hash() override;
    ProjNode* proj(int idx);
};
#endif