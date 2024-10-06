#include "../Include/graph_visualizer.h"

// Function implementations
std::string GraphVisualizer::generateDotOutput(Parser &parser) {
  std::vector<Node *> all = findAll(parser);
  std::ostringstream sb;
  sb << "digraph chapter03 {\n";
  sb << "/*\n";
  sb << parser.src();
  sb << "\n*/\n";
  sb << "\trankdir=BT;\n";
  sb << "\tordering=\"in\";\n";
  sb << "\tconcentrate=\"true\";\n";
  nodes(sb, all);
  nodeEdges(sb, all);
  sb << "}\n";
  return sb.str();
}

void GraphVisualizer::nodes(std::ostringstream &sb,
                            const std::vector<Node *> &all) {
  sb << "\tsubgraph cluster_Nodes {\n";
  for (Node *n : all) {
    sb << "\t\t" << n->uniqueName() << " [ ";
    std::string lab = n->glabel();
    if (n->isCFG()) {
      sb << "shape=box style=filled fillcolor=yellow ";
    }
    sb << "label=\"" << lab << "\" ";
    sb << "];\n";
  }
  sb << "\t}\n";
}

void GraphVisualizer::nodeEdges(std::ostringstream &sb,
                                const std::vector<Node *> &all) {
  sb << "\tedge [ fontname=Helvetica, fontsize=8 ];\n";
  for (Node *n : all) {
    int i = 0;
    for (Node *def : n->inputs) {
      if (def != nullptr) {
        sb << "\t" << n->uniqueName() << " -> " << def->uniqueName();
        sb << "[taillabel=" << i;
        if (dynamic_cast<ConstantNode *>(n) && dynamic_cast<StartNode *>(def)) {
          sb << " style=dotted";
        } else if (def->isCFG()) {
          sb << " color=red";
        }
        sb << "];\n";
      }
      i++;
    }
  }
}

std::vector<Node *> GraphVisualizer::findAll(Parser &parser) {
  StartNode *start = Parser::START;
  std::unordered_map<int, Node *> all;
  for (Node *n : start->outputs) {
    walk(all, n);
  }
  std::vector<Node *> result;
  for (auto &entry : all) {
    result.push_back(entry.second);
  }
  return result;
}

void GraphVisualizer::walk(std::unordered_map<int, Node *> &all, Node *n) {
  if (all.find(n->nid) != all.end())
    return;
  all[n->nid] = n;

  for (Node *c : n->inputs) {
    if (c != nullptr) {
      walk(all, c);
    }
  }
  for (Node *c : n->outputs) {
    if (c != nullptr) {
      walk(all, c);
    }
  }
}