#include "../Include/graph_visualizer.h"

// Function implementations
std::string GraphVisualizer::generateDotOutput(Parser &parser) {
  std::vector<Node *> all = findAll(parser);
  std::ostringstream sb;
  sb << "digraph chapter06 {\n";
  sb << "/*\n";
  sb << parser.src();
  sb << "\n*/\n";
  sb << "\trankdir=BT;\n";
  sb << "\tordering=\"in\";\n";
  sb << "\tconcentrate=\"true\";\n";
  // Just the Nodes first, in a cluster no edges
  nodes(sb, all);

  // Now the scopes, in a cluster no edges
  for (auto sn : parser.xScopes)
    scopes(sb, sn);
  // Walk the node edges
  nodeEdges(sb, all);

  // Walk the scope edges
  for (auto sn : parser.xScopes)
    scopeEdges(sb, sn);

  sb << "}\n";
  return sb.str();
}

void GraphVisualizer::nodes(std::ostringstream &sb,
                            const std::vector<Node *> &all) {
  sb << "\tsubgraph cluster_Nodes {\n";
  for (Node *n : all) {
    if (dynamic_cast<ProjNode *>(n) || dynamic_cast<ScopeNode *>(n))
      continue; // Do not emit, rolled into MultiNode or Scope cluster already

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
  for (auto n : all) {
    if (auto *proj = dynamic_cast<RegionNode *>(n)) {
      sb << "\t\t{ rank=same; ";
      sb << proj->print_1(sb).str();
      sb << ";";
      for (auto phi : proj->outputs) {
        if (auto *proj = dynamic_cast<PhiNode *>(phi)) {
          sb << phi->uniqueName() << ";";
        }
        sb << "}\n";
      }
    }
  }
  sb << "\t}\n";
}

void GraphVisualizer::nodeEdges(std::ostringstream &sb,
                                const std::vector<Node *> &all) {
  sb << "\tedge [ fontname=Helvetica, fontsize=8 ];\n";
  for (Node *n : all) {
    // Do not display the Constant->Start edge;
    // ProjNodes handled by Multi;
    // ScopeNodes are done separately
    if (dynamic_cast<ProjNode *>(n) || dynamic_cast<ScopeNode *>(n) ||
        dynamic_cast<ProjNode *>(n))
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
        sb << "];\n";
      }
      i++;
    }
  }
}

void GraphVisualizer::scopes(std::ostringstream &sb, ScopeNode *n) {
  sb << "\tnode [shape=plaintext];\n";
  int level = 0;
  for (auto &scope : n->scopes) {
    std::string scopeName = makeScopeName(n, level);
    sb << "\tsubgraph cluster_" << scopeName << " {\n";
    sb << "\t\tlabel=\"" << scopeName << " [label=<\n";
    sb << "\t\t\t<TABLE BORDER=\"0\" CELLBORDER=\"1\" CELLSPACING=\"0\">\n";
    // Add the scope level
    sb << "\t\t\t<TR><TD BGCOLOR=\"cyan\">" << level << "</TD>\n";
    for (const auto &pair : scope) {
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
  for (int i = 0; i < level; ++i) {
    sb << "\t}\n"; // End all Scope clusters
  }
}

void GraphVisualizer::scopeEdges(std::ostringstream &sb, ScopeNode *n) {
  sb << "\tedge [style=dashed color=cornflowerblue];\n";
  int level = 0;
  for (const auto &scope : n->scopes) {
    std::string scopeName = makeScopeName(n, level);
    for (const auto &pair : scope) {
      std::string name = pair.first;
      Node *def = n->in(pair.second);
      if (def == nullptr)
        continue;
      sb << scopeName << ":" << '"' << makePortName(scopeName, name) << '"'
         << " -> ";
      if (auto *proj = dynamic_cast<ProjNode *>(def)) {
        std::string mname = proj->ctrl()->uniqueName();
        sb << mname << ":p" << proj->idx_;
      } else
        sb << def->uniqueName();
      sb << ";";
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
