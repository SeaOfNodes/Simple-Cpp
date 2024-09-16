#ifndef GRAPH_VISUALIZER_H
#define GRAPH_VISUALIZER_H

#include <string>
#include <vector>
#include <unordered_map>
#include <sstream>

#include "../Include/node/node.h"    // Assuming Node and other necessary headers are defined
#include "../Include/parser.h"

class GraphVisualizer {
public:
  std::string generateDotOutput(Parser& parser);  // Generate the dot file as a string

private:
  void nodes(std::ostringstream& sb, const std::vector<Node*>& all);  // Add nodes to dot format
  void nodeEdges(std::ostringstream& sb, const std::vector<Node*>& all);  // Define node edges
  std::vector<Node*> findAll(Parser& parser);  // Find all nodes in the graph
  void walk(std::unordered_map<int, Node*>& all, Node* n);  // Recursive graph traversal
};

#endif // GRAPH_VISUALIZER_H