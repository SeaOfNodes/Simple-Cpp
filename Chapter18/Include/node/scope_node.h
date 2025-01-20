#ifndef SCOPE_NODE_H
#define SCOPE_NODE_H

#include "../../Include/node/node.h"
#include "../../Include/node/phi_node.h"
#include "../../Include/node/region_node.h"
#include "../../Include/type/type.h"
#include "../../Include/type/type_mem_ptr.h"

#include "../../Include/node/not_node.h"
#include "../../Include/node/scope_minnode.h"
#include "../../Include/node/mem_merge_node.h"

#include <stack>
#include <unordered_map>

class ScopeNode : public MergeMemNode {
public:
    /**
     * A Stack of symbol tables; a symbol table is a map from names to node
     * indices.
     */
    /*
     * The Stack represents a nested scope hierarchy. The deeper the scope, the
     * higher it is on the stack.
     * If we want to look up then just "nestedlevel - 1"
     * You eventually end up with multiple scopes same level - but they are not
     * alive at the same time.
     * */
    static std::string CTRL;
    static std::string ARG0;
    static std::string MEM0;


    // All active/live variables in all nested scopes, all run together
    Tomi::Vector<ScopeMinNode::Var*> vars;

    // Read from memory
    MergeMemNode *mem();
    // Write to memory
    Node* mem(int alias);

    Node* mem(Node*n);
    void mem(int alias, Node*st);

    bool outOfDefinition(ScopeMinNode::Var*v);

    // Look for forward references in the last lexical scope and promote to the
    // next outer lexical scope.  At the last scope declare them an error.
    void promote();
    // Size of each nested lexical scope
    Tomi::Vector<int> lexSize;

    enum Kind{Block, Constructor, Function};
    Tomi::Vector<Kind> kinds_;



    // Extra guards; tested predicates and casted results
    Tomi::Vector<Node*> _guards;

    void merge_(ScopeNode* that, RegionNode* r);

    // Find name in reverse, return an index into _vars or -1.  Linear scan
    // instead of hashtable, but probably doesn't matter until the scan
    // typically hits many dozens of variables.
    int find(std::string name);

    void balanceIf(ScopeNode* scope);
//  Tomi::Vector<Tomi::HashMap<std::string, int>> idxs;
    Tomi::Vector<std::string> keys;

    /**
   * Tracks declared types for every name
   */
//    Tomi::Vector<Tomi::HashMap<std::string, Type*>> decls;
    ScopeNode();
    // Up-casting: using the results of an If to improve a value.
    // E.g. "if( ptr ) ptr.field;" is legal because ptr is known not-null.

    // This Scope looks for direct variable uses, or certain simple
    // combinations, and replaces the variable with the upcast variant.

    Node *upcast(Node *ctrl, Node *pred, bool invert);

    std::string label() override;

    Type *compute() override;

    Node *idealize() override;

    std::ostringstream &print_1(std::ostringstream &builder, Tomi::Vector<bool> &visited) override;

    /*
   * * Lookup a name in all scopes starting from most deeply nested.
   * */
    ScopeMinNode::Var* lookup(std::string name);

    Type *lookUpDeclaredType(std::string name);

    Node *replace(Node *old, Node *cast);

    /*
     * Create a new name in the current scope
     * Check if name already exists in the current scope
     */
    bool define(std::string name, Type *declaredType, bool xfinal, Node *n);

    /**
     * If the name is present in any scope, then redefine else null
     * The nestingLevel is going to be the current scope level
     * <p>
     * @param name Name being redefined
     * @param n    The node to bind to the name
     */
    void update(std::string name, Node *n);

    ScopeMinNode::Var* update(ScopeMinNode::Var* v, Node*st);
    /**
     * Both recursive lookup and update.
     * <p>
     * A shared implementation allows us to create lazy phis both during
     * lookups and updates; the lazy phi creation is part of chapter 8.
     *
     * @param name  Name whose binding is being updated or looked up
     * @param n     If null, do a lookup, else update binding
     * @param nestingLevel The starting nesting level
     * @return node being looked up, or the one that was updated
     */

    /**
     * Recover the names for all variable bindings.
     * The result is an array of names that is aligned with the
     * inputs to the Node.
     *
     * This is an expensive operation.
     */
    Tomi::Vector<std::string> reverseNames();

    Node *ctrl();
    /**
     * The ctrl of a ScopeNode is always bound to the currently active
     * control node in the graph, via a special name '$ctrl' that is not
     * a valid identifier in the language grammar and hence cannot be
     * referenced in Simple code.
     *
     * @param n The node to be bound to '$ctrl'
     *
     * @return Node that was bound
     */

    /**
     * Duplicate a ScopeNode; including all levels, up to Nodes.  So this is
     * neither shallow (would dup the Scope but not the internal HashMap
     * tables), nor deep (would dup the Scope, the HashMap tables, but then
     * also the program Nodes).
     * <p>
     * The new Scope is a full-fledged Node with proper use<->def edges.
     */
    ScopeNode *dup();

    ScopeNode *dup(bool loop);

    /**
     * Merges the names whose node bindings differ, by creating Phi node for such
     * names The names could occur at all stack levels, but a given name can only
     * differ in the innermost stack level where the name is bound.
     *
     * @param that The ScopeNode to be merged into this
     * @return A new node representing the merge point
     */
    RegionNode *mergeScopes(ScopeNode *that, Lexer* loc);

    void addGuards(Node* ctlr, Node* pred, bool invert);

    // Remove matching pred/cast pairs from this guarded region.
    void removeGuards(Node* ctrl);

    Node* upcastGuard(Node* pred);
    void kill();
    // Merge the backedge scope into this loop head scope
    // We set the second input to the phi from the back edge (i.e. loop body)
    void endLoop(ScopeNode *back, ScopeNode *exit);
    void endLoopMem_(ScopeNode*scope, Node*back, Node*exit);

    /**
   * The ctrl of a ScopeNode is always bound to the currently active
   * control node in the graph, via a special name '$ctrl' that is not
   * a valid identifier in the language grammar and hence cannot be
   * referenced in Simple code.
   *
   * @param n The node to be bound to '$ctrl'
   *
   * @return Node that was bound
   */
    Node *ctrl(Node *n);

    void push();
    bool inCon();
    void push(bool InCon);
    void pop();
};

#endif