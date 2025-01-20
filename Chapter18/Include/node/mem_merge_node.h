#ifndef MEM_MERGE_NODE_H
#define MEM_MERGE_NODE_H

#include "node.h"
#include "scope_minnode.h"
/**
 *  Memory Merge - a merge of many aliases into a "fat memory".  All aliases
 *  are here, but most will be lazy - take the default fat memory.
 */
class MergeMemNode: public Node {
    /*
 *  In-Progress means this is being used by the Parser to track memory
 *  aliases.  No optimizations are allowed.  When no longer "in progress"
 *  normal peeps work.
 */
    bool inProgress_;
public:
MergeMemNode(bool inProgress);
bool inProgress();
std::string label() override;
bool isMem() override;
std::ostringstream& print_1(std::ostringstream& builder, Tomi::Vector<bool>& visited) override;
Node* merge();
Type*compute() override;
Node*idealize() override;

// Todo: why is this happening
// name hiding BS
using Node::in;

Node* in(ScopeMinNode::Var* v);
Node*alias(int alias);
Node*alias(int alias, Node*st);
// Read or update from memory.
// A shared implementation allows us to create lazy phis both during
// lookups and updates; the lazy phi creation is part of chapter 8.
Node* mem_(int alias, Node*st);
void merge_(MergeMemNode* that, RegionNode* r);
// Fill in the backedge of any inserted Phis
void endLoopMem_(ScopeNode* scope, MergeMemNode*back, MergeMemNode* exit);
// Now one-time do a useless-phi removal
void useless_();
bool eq(Node*n) override;
};
#endif