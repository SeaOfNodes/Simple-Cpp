#include "../../Include/node/scope_node.h"

ScopeNode::ScopeNode() : Node({}){ type_ = &Type::BOTTOM; }
std::string ScopeNode::label() { return "Scope"; }
Type *ScopeNode::compute() { return &Type::BOTTOM; }
Node *ScopeNode::idealize() { return nullptr; }

void ScopeNode::push() {
  scopes.push_back(std::unordered_map<std::string, int>{});
}
void ScopeNode::pop() {
  // first pop elements in hashmap
  popN(scopes.back().size());
  // then pop the empty hashmap
  scopes.pop_back();
}

// add it here
Node *ScopeNode::define(std::string name, Node *n) {
  if (!scopes.empty()) {
    auto &sysm = scopes.back();

    if (sysm.find(name) != sysm.end())
      return nullptr; // double define

    sysm[name] = nIns();
  }
  return addDef(n);
}

Node *ScopeNode::lookup(std::string name) {
  return update(name, nullptr, scopes.size() - 1);
}

Node *ScopeNode::update(std::string name, Node *n) {
  return update(name, n, scopes.size() - 1);
}

Node *ScopeNode::update(std::string name, Node *n, int nestingLevel) {
  // nesting level is negative if nothing is found
  if (nestingLevel < 0) // Missed in all scopes, not found
    return nullptr;

  auto syms = scopes[nestingLevel]; // Get the symbol table for nesting level
  auto idx = syms.find(name);
  // Not found in this scope, recursively look in parent scope
  if (idx == syms.end())
    return update(name, n, nestingLevel - 1);
  Node *old = in(idx->second);
  // If n is null we are looking up rather than updating, hence return existing
  // value
  return n == nullptr ? old : setDef(idx->second, n);
}

Node *ScopeNode::ctrl() { return in(0); }

Node *ScopeNode::ctrl(Node *n) { return setDef(0, n); }

std::ostringstream &ScopeNode::print_1(std::ostringstream &builder) {
  builder << label();
  keys.reserve(scopes.size());

  for (auto scope : scopes) {
    builder << "[";
    bool first = true;
    for (const auto &pair : scope) {
      if (!first)
        builder << ", ";
      first = false;
      std::string name = pair.first;
      builder << name << ":";

      Node *n = in(pair.second);
      if (n == nullptr)
        builder << "nullptr";
      else
        n->print_0(builder);
    }
    builder << "]";
  }

  return builder;
}

Node *ScopeNode::mergeScopes(ScopeNode *that) {
  RegionNode *r = (RegionNode *)ctrl(
      (new RegionNode({nullptr, ctrl(), that->ctrl()}))->peephole());
  std::vector<std::string> ns = reverseNames();
  // Note that we skip i==0, which is bound to '$ctrl'
  for (int i = 1; i < nIns(); i++) {
    if (in(i) != that->in(i)) { // No need for redundant Phis
      Node *phi = new PhiNode(ns[i], {r, in(i), that->in(i)});
      phi = phi->peephole();
      setDef(i, phi);
    }
  }
  that->kill();
  return r;
}

std::vector<std::string> ScopeNode::reverseNames() {
  std::vector<std::string> names(nIns());
  for (const auto &syms : scopes) {
    for (const auto &pair : syms) {
      names[pair.second] = pair.first;
    }
  }
  return names;
}
std::string ScopeNode::CTRL = "$ctrl";
std::string ScopeNode::ARG0 = "arg";