#ifndef SCOPEMIN_NODE_H
#define SCOPEMIN_NODE_H

#include "node.h"
#include "region_node.h"

class ScopeNode;

class ScopeMinNode : public Node {
    /** The tracked fields are now complex enough to deserve a array-of-structs layout
 */
public:
     class Var {
     public:
         int idx_;
         std::string name_;
         Type*type_;
         bool final_;
        Var(int idx, std::string name, Type* type, bool final);
    };
    ScopeMinNode();

    std::string label() override;

    std::ostringstream  &print_1(std::ostringstream &builder, Tomi::Vector<bool> &visited) override;

    Type *compute() override;

    Node *idealize() override;

    using Node::in; // bring in the hidden function from the base class(e.g when an integer as passed in as the paramter)
    Node *in(Var* v);

    Node *alias(int alias);

    Node *alias(int alias, Node *st);

    // Read or update from memory.
    // A shared implementation allows us to create lazy phis both during
    // lookups and updates; the lazy phi creation is part of chapter 8.
    Node *mem_(int alias, Node *st);

    void endLoop(ScopeNode* bakc, ScopeNode* exit);
    void endLoop_(ScopeNode* scope, Node* back, Node*exit);

    void merge_(ScopeMinNode *that, RegionNode *r);

    // Fill in the backedge of any inserted Phis
    void endLoopMem_(ScopeNode *scope, ScopeMinNode *back, ScopeMinNode *exit);

    // Now one-time do a useless-phi removal
    void useless_();

};

#endif