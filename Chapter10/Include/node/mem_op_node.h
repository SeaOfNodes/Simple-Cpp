#ifndef MEM_OP_NODE_H
#define MEM_OP_NODE_H

/**
 * Convenience common base for Load and Store.
 */
#include "node.h"

class MemOpNode : public Node {
public:
    std::string name_;
    int alias_;
    MemOpNode(std::string name, int alias, Node* memSlice, Node* memPtr, Node* value);
    Node* mem();
    Node* ptr();

    bool eq(Node* n) override;
    int hash() override;
    std::string err() override;
};

#endif