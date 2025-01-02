#ifndef SCOPE_NODE_H
#define SCOPE_NODE_H
#include "../../Include/node/node.h"
#include "../../Include/node/phi_node.h"
#include "../../Include/node/region_node.h"
#include "../../Include/type/type.h"
#include "../../Include/type/type_mem_ptr.h"

#include "../../Include/node/not_node.h"

#include <stack>
#include <unordered_map>
class ScopeNode : public Node {
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

  Tomi::Vector<Tomi::HashMap<std::string, int>> idxs;
  Tomi::Vector<std::string> keys;

    /**
   * Tracks declared types for every name
   */
    Tomi::Vector<Tomi::HashMap<std::string, Type*>> decls;
  ScopeNode();
  // Up-casting: using the results of an If to improve a value.
  // E.g. "if( ptr ) ptr.field;" is legal because ptr is known not-null.

  // This Scope looks for direct variable uses, or certain simple
  // combinations, and replaces the variable with the upcast variant.

  Node* upcast(Node* ctrl, Node* pred, bool invert);

  std::string label() override;
  Type *compute() override;
  Node *idealize() override;
  std::ostringstream &print_1(std::ostringstream &builder, Tomi::Vector<bool>& visited) override;

    /*
   * * Lookup a name in all scopes starting from most deeply nested.
   * */
  Node *lookup(std::string name);

  Type* lookUpDeclaredType(std::string name);

  Node* replace(Node* old, Node* cast);
  /*
   * Create a new name in the current scope
   * Check if name already exists in the current scope
   */
  Node *define(std::string name, Type* declaredType, Node *n);
  /**
   * If the name is present in any scope, then redefine else null
   * The nestingLevel is going to be the current scope level
   * <p>
   * @param name Name being redefined
   * @param n    The node to bind to the name
   */
  Node *update(std::string name, Node *n);
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
  Node *update(std::string name, Node *n, int nestingLevel);

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
  Node *mergeScopes(ScopeNode *that);
  // Merge the backedge scope into this loop head scope
  // We set the second input to the phi from the back edge (i.e. loop body)
  void endLoop(ScopeNode* back, ScopeNode* exit);

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
  void pop();
};
#endif