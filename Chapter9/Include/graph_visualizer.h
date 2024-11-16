#ifndef GRAPH_VISUALIZER_H
#define GRAPH_VISUALIZER_H

// if not true then DEF_USE
#define USE_DEF true
#include <string>
#include <vector>
#include <unordered_map>
#include <sstream>

#include "../Include/node/node.h"    // Assuming Node and other necessary headers are defined
#include "../Include/parser.h"
#include "../Include/node/proj_node.h"

class GraphVisualizer {
public:
  /**
     * If set to true we put the control nodes in a separate cluster from
     * data nodes.
   */
  bool separateControlCluster = false;
  std::string generateDotOutput(Parser &parser); // Generate the dot file as a string
/*  std::string generateDotOutput(StopNode* stop, Node* scope, S)*/
  GraphVisualizer();
  GraphVisualizer(bool separateControlCluster);
private:

    void nodesByCluster(std::ostringstream& sb, bool doCtrl, const Tomi::Vector<Node*> &all);

    void nodes(std::ostringstream &sb, const Tomi::Vector<Node *> &all); // Add nodes to dot format
    void nodeEdges(std::ostringstream &sb, const Tomi::Vector<Node *> &all); // Define node edges
    Tomi::Vector<Node *> findAll(Parser &parser); // Find all nodes in the graph
    void walk(std::unordered_map<int, Node *> &all, Node *n); // Recursive graph traversal

    void scopes(std::ostringstream &sb, ScopeNode *n);

    void scopeEdges(std::ostringstream &b, ScopeNode *n);

    std::string makeScopeName(ScopeNode *n, int level);

    std::string makePortName(std::string ScopeNode, std::string varName);
};

#endif // GRAPH_VISUALIZER_H
