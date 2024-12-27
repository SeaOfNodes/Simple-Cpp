#ifndef MEM_OP_NODE_H
#define MEM_OP_NODE_H

/**
 * Convenience common base for Load and Store.
 */
#include "node.h"

class MemOpNode : public Node {
public:
    MemOpNode() = default;
    std::string name_;
    Type* declaredType;
    int alias_{};
    MemOpNode(std::string name, int alias, Type*glb, std::initializer_list<Node*> nodes);
    Node* mem();
    Node* ptr();

    bool eq(Node* n) override;
    int hash() override;
    std::string err() override;
};

#endif