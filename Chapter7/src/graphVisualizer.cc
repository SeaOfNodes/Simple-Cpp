#include "../Include/graph_visualizer.h"

/*
 * DOT notes:
 * directed vs undirected:
 * diagraph specifies the graph is directed: meaning that the arrows(edges) must
point
 * towards a direction.
 * e.g
 * digraph directedGraph {
  A -> B;
  A -> C;
  C -> B;
  C -> D;
  }
  VS

graph undirectedGraph {
A -- B;
A -- C;
B -- C;
C -- D;
}
//
vs

Comments same way as cpp or java.

SubGraphs play 3 roles:
subgraph can be used to represent a graph structure, indicating the certain
nodes and edges should be grouped together. Can provide context for setting
attributes.

If the name of the subgraph begins with cluster, Graphviz notes the subgraph as
a special cluster subgraph. e.g it surrounds it with a rectangle

Nodes can be created inside a sub-graph and then modified with attributes.
e.g: Con_7 [ label="12" ];


}
Child-Parent relationship between the graphs(nested)

 subgraph cluster_Scope1_1 {
        Scope1_1 [label=<
                <TABLE BORDER="0" CELLBORDER="1" CELLSPACING="0">
                <TR><TD BGCOLOR="cyan">1</TD><TD PORT="Scope1_1_a">a</TD><TD
PORT="Scope1_1_b">b</TD></TR>
                </TABLE>>];
subgraph cluster_Scope1_2 {
        Scope1_2 [label=<
                <TABLE BORDER="0" CELLBORDER="1" CELLSPACING="0">
                <TR><TD BGCOLOR="cyan">0</TD><TD
PORT="Scope1_2_$ctrl">$ctrl</TD><TD PORT="Scope1_2_arg">arg</TD></TR>
                </TABLE>>];
}
}

Edges:
node -> node2
In our case we are drawing an arrow from
 Scope1_1:"Scope1_1_a" -> Con_6; where Scope1_1 is the subgraph and "Scope1_1_a"
is the specific port while Con_6 is another node.
 * */

GraphVisualizer::GraphVisualizer(bool separateControlCluster)
    : separateControlCluster(separateControlCluster) {}
GraphVisualizer::GraphVisualizer() : GraphVisualizer(false) {}

std::string GraphVisualizer::generateDotOutput(Parser &parser) {
  std::vector<Node *> all = findAll(parser);
  std::ostringstream sb;
  sb << "digraph chapter07 {\n";
  sb << "/*\n";
  sb << parser.src();
  sb << "\n*/\n";
  sb << "\trankdir=BT;\n";
  sb << "\tordering=\"in\";\n";
  sb << "\tconcentrate=\"true\";\n";
  sb << "\tcompound=\"true\";\n";
  // Just the Nodes first, in a cluster no edges

  nodes(sb, all);

  // Now the scopes, in a cluster no edges
  for (auto sn : parser.xScopes) {
    scopes(sb, sn);
  }

  // Walk the node edges
  nodeEdges(sb, all);

  // Walk the scope edges
  for (auto sn : parser.xScopes) {
    scopeEdges(sb, sn);
  }

  sb << "}\n";
  return sb.str();
}

void GraphVisualizer::nodesByCluster(std::ostringstream &sb, bool doCtrl,
                                     const std::vector<Node *> &all) {
  if (!separateControlCluster && doCtrl)
    return; // all nodes in 1 cluster
  sb << (doCtrl ? "\tsubgraph cluster_Controls {\n"
                : "\tsubgraph cluster_Nodes {\n");
  for (Node *n : all) {
    if (dynamic_cast<ProjNode *>(n) || dynamic_cast<ScopeNode *>(n))
      continue; // Do not emit, rolled into MultiNode or Scope cluster already
    if (separateControlCluster && doCtrl && !n->isCFG())
      continue;
    if (separateControlCluster && !doCtrl && n->isCFG())
      continue;
    sb << "\t\t" << n->uniqueName() << " [ ";
    std::string lab = n->glabel();
    if (dynamic_cast<MultiNode *>(n)) {
      sb << "shape=plaintext label=<\n";
      sb << "\t\t\t<TABLE BORDER=\"0\" CELLBORDER=\"1\" CELLSPACING=\"0\" "
            "CELLPADDING=\"4\">\n";
      sb << "\t\t\t<TR><TD BGCOLOR=\"yellow\">" << lab << "</TD></TR>\n";
      sb << "\t\t\t<TR>";
      bool doProjTable = false;
      for (auto *use : n->outputs) {
        if (auto *proj = dynamic_cast<ProjNode *>(use)) {
          if (!doProjTable) {
            doProjTable = true;
            sb << "<TD>"
               << "\n";
            sb << "\t\t\t\t<TABLE BORDER=\"0\" CELLBORDER=\"1\" "
                  "CELLSPACING=\"0\">"
               << "\n";
            sb << "\t\t\t\t<TR>";
          }
          sb << "<TD PORT=\"p" << proj->idx_ << "\"";
          if (proj->isCFG())
            sb << " BGCOLOR=\"yellow\"";
          sb << ">" << proj->glabel() << "</TD>\n";
        }
      }
      if (doProjTable) {
        sb << "</TR>";
        sb << "\t\t\t\t</TABLE>"
           << "\n";
        sb << "\t\t\t</TD>";
      }
      sb << "</TR>"
         << "\n";
      sb << "\t\t\t</TABLE>>\n\t\t";
    } else {
      if (n->isCFG()) {
        sb << "shape=box style=filled fillcolor=yellow ";
      }
      if (auto *proj = dynamic_cast<PhiNode *>(n))
        sb << "style=filled fillcolor=lightyellow";
      sb << "label=\"" << lab << "\" ";
    }
    sb << "];\n";
  }
  // Force Region & Phis to line up
  if (!separateControlCluster) {
    for (auto n : all) {
      if (auto *proj = dynamic_cast<RegionNode *>(n)) {
        sb << "\t\t{ rank=same; ";
        sb << proj->print_0(sb, {}).str();
        sb << ";";
        for (auto phi : proj->outputs) {
          if (auto *proj = dynamic_cast<PhiNode *>(phi)) {
            sb << phi->uniqueName() << ";";
          }
          sb << "}\n";
        }
      }
    }
  }
  sb << "\t}\n";
}
void GraphVisualizer::nodes(std::ostringstream &sb,
                            const std::vector<Node *> &all) {
  nodesByCluster(sb, true, all);
  nodesByCluster(sb, false, all);
}
void GraphVisualizer::nodeEdges(std::ostringstream &sb,
                                const std::vector<Node *> &all) {
  sb << "\tedge [ fontname=Helvetica, fontsize=8 ];\n";
  for (Node *n : all) {
    // Do not display the Constant->Start edge;
    // ProjNodes handled by Multi;
    // ScopeNodes are done separately
    if (dynamic_cast<ProjNode *>(n) || dynamic_cast<ScopeNode *>(n) ||
        dynamic_cast<ConstantNode *>(n))
      continue;
    int i = 0;
    for (Node *def : n->inputs) {
      if (dynamic_cast<PhiNode *>(n) && dynamic_cast<RegionNode *>(def)) {
        sb << '\t';
        sb << n->uniqueName();
        sb << " -> ";
        sb << " [style=dotted taillabel=" << i << "];\n";
      } else if (def != nullptr) {
        sb << '\t' << n->uniqueName() << " -> ";
        if (auto *proj = dynamic_cast<ProjNode *>(def)) {
          std::string name = proj->ctrl()->uniqueName();
          sb << name << ":p" << proj->idx_;
        } else
          sb << def->uniqueName();
        sb << "[taillabel=" << i;
        if (def->isCFG())
          sb << " color=red";
        if(i == 2 && (dynamic_cast<PhiNode*>(n) || dynamic_cast<LoopNode*>(n))) sb << " constraint=false";
        sb << "];\n";
      }
      i++;
    }
  }
}

void GraphVisualizer::scopes(std::ostringstream &sb, ScopeNode *n) {
  sb << "\tnode [shape=plaintext];\n";
  int level = 1;
  for (int idx = n->scopes.size() - 1; idx >= 0; idx--) {
    auto sysms = n->scopes[idx];
    std::string scopeName = makeScopeName(n, level);
    sb << "\tsubgraph cluster_" << scopeName << " {\n";
    sb << scopeName << " [label=<\n";
    sb << "\t\t\t<TABLE BORDER=\"0\" CELLBORDER=\"1\" CELLSPACING=\"0\">\n";
    // Add the scope level
    int scopeLevel = n->scopes.size() - level;
    sb << "\t\t\t<TR><TD BGCOLOR=\"cyan\">" << scopeLevel << "</TD>\n";

    for (const auto &pair : sysms) {
      const std::string &name = pair.first;
      sb << "<TD PORT=\"" << makePortName(scopeName, name) << "\">" << name
         << "</TD>";
    }

    sb << "</TR>\n";
    sb << "\t\t\t</TABLE>>];\n";
    level++;
  }
  // Scope clusters nest, so the graphics shows the nested scopes, so
  // they are not closed as they are printed; so they just keep nesting.
  // We close them all at once here.
  for (int i = 0; i < level - 1; ++i) {
    sb << "\t}\n"; // End all Scope clusters
  }
}

void GraphVisualizer::scopeEdges(std::ostringstream &sb, ScopeNode *n) {
  sb << "\tedge [style=dashed color=cornflowerblue];\n";
  int level = 1;
  for (std::size_t i = n->scopes.size() - 1; i != -1; --i) {
    auto syms = n->scopes[i];
    std::string scopeName = makeScopeName(n, level);
    for (const auto &pair : syms) {
      std::string name = pair.first;
      int idx = syms[name];
      Node *def = n->in(idx);
      std::string unique_name = def->uniqueName();
      if (def == nullptr)
        continue;
      sb << "\t" << scopeName << ":" << '"' << makePortName(scopeName, name)
         << '"' << " -> ";
      if (auto *proj = dynamic_cast<ProjNode *>(def); proj) {
        std::string mname = proj->ctrl()->uniqueName();
        sb << mname << ":p" << proj->idx_;
      } else {
        sb << def->uniqueName();
      }
      sb << ";\n";
    }
    level++;
  }
}

std::string GraphVisualizer::makeScopeName(ScopeNode *n, int level) {
  return n->uniqueName() + "_" + std::to_string(level);
}

std::string GraphVisualizer::makePortName(std::string ScopeName,
                                          std::string varName) {
  return ScopeName + "_" + varName;
}

std::vector<Node *> GraphVisualizer::findAll(Parser &parser) {
  std::unordered_map<int, Node *> all;
  for (Node *n : Parser::START->outputs) {
    walk(all, n);
  }
  for (auto n : parser.scope_node->inputs) {
    // std::cout << n->uniqueName() << "\n";
    walk(all, n);
  }
  std::vector<Node *> result;
  for (auto &entry : all) {
    result.push_back(entry.second);
  }
  // is the order going to be different here?
  /*  for(const auto& pair: all) {
      Node*node = pair.second;
      std::cout << node->uniqueName() << "\n";
    }*/
  return result;
}

void GraphVisualizer::walk(std::unordered_map<int, Node *> &all, Node *n) {
  if (!n)
    return;
  if (all.find(n->nid) != all.end())
    return;

  all[n->nid] = n;

  for (Node *c : n->inputs) {
    walk(all, c);
  }
  for (Node *c : n->outputs) {
    walk(all, c);
  }
}
