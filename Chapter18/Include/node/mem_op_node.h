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
    //  // Declared type; not final because it might be a forward-reference
    //    // which will be lazily improved when the reference is declared.
    MemOpNode(std::string name, int alias, Type*glb, Node* mem, Node*ptr, Node*off);
    MemOpNode(std::string name, int alias, Type*glb, Node*mem, Node*ptr, Node*off, Node*val);

    Node* mem();
    Node* ptr();
    Node* off();

    static std::string mlabel(std::string name);
    std::string mlabel();

    bool eq(Node* n) override;
    int hash() override;
    std::string err() override;
};

#endif