#ifndef SCOPE_NODE_H
#define SCOPE_NODE_H
#include "../../Include/node/node.h"
#include "../../Include/type/type.h"

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
  ;

  std::vector<std::unordered_map<std::string, int>> scopes;
  std::vector<std::string> keys;

  ScopeNode();

  std::string label() override;
  Type *compute() override;
  Node *idealize() override;
  std::ostringstream &print_1(std::ostringstream &builder) override;

  /*
   * * Lookup a name in all scopes starting from most deeply nested.
   * */
  Node *lookup(std::string name);

  /*
   * Create a new name in the current scope
   * Check if name already exists in the current scope
   */
  Node *define(std::string name, Node *n);
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
  ScopeNode *dup() {
    ScopeNode *dup = new ScopeNode();
    // Our goals are:
    // 1) duplicate the name bindings of the ScopeNode across all stack levels
    // 2) Make the new ScopeNode a user of all the nodes bound
    // 3) Ensure that the order of defs is the same to allow easy merging
    for (auto syms : scopes) {
      dup->scopes.push_back(syms);
    }
    // Control comes first
    dup->addDef(ctrl());
    // now, all the inputs
    for (int i = 1; i < nIns(); i++) {
      dup->addDef(in(i));
    }
    return dup;
  }
  Node *ctrl(Node *n);
  void push();
  void pop();
};
#endif