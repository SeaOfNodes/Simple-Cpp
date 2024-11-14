#include "../../Include/node/scope_node.h"
#include <cassert>

ScopeNode::ScopeNode() : Node({}) { type_ = Type::BOTTOM; }
std::string ScopeNode::label() { return "Scope"; }
Type *ScopeNode::compute() { return Type::BOTTOM; }
Node *ScopeNode::idealize() { return nullptr; }

void ScopeNode::push() { scopes.emplace_back(); }
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

    sysm[name] = static_cast<int>(nIns());
  }
  return addDef(n);
}

Node *ScopeNode::lookup(std::string name) {
  return update(std::move(name), nullptr, static_cast<int>(scopes.size() - 1));
}

Node *ScopeNode::update(std::string name, Node *n) {
  return update(std::move(name), n, static_cast<int>(scopes.size() - 1));
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
  if (auto *loop = dynamic_cast<ScopeNode *>(old)) {
    // Lazy Phi!
    auto *phi = dynamic_cast<PhiNode *>(
        loop->in(static_cast<std::size_t>(idx->second)));
    if (phi && loop->ctrl() == phi->region()) {
      old = loop->in(static_cast<std::size_t>(idx->second));
    } else {
      old = loop->setDef(
          idx->second,
          (new PhiNode(name,
                       {loop->ctrl(), loop->update(name, nullptr, nestingLevel),
                        nullptr}))
              ->peephole());
    }
    setDef(idx->second, old);
  }
  // If n is null we are looking up rather than updating, hence return existing
  // value
  return n == nullptr ? old : setDef(idx->second, n);
}

Node *ScopeNode::ctrl() { return in(0); }

Node *ScopeNode::ctrl(Node *n) { return setDef(0, n); }

std::ostringstream &ScopeNode::print_1(std::ostringstream &builder,
                                       Tomi::Vector<bool> &visited) {
  builder << label();
  builder << "[";
  keys.reserve(scopes.size());
  Tomi::Vector<std::string> names = reverseNames();
  for (int j = 0; j < nIns(); j++) {
    builder << names[j] << ":";
    Node *n = in(j);
    while (auto *loop = dynamic_cast<ScopeNode *>(n)) {
      builder << "Lazy_";
      n = loop->in(j);
    }
    n->print_0(builder, visited);
  }
  builder << "]";

  return builder;
}

Node *ScopeNode::mergeScopes(ScopeNode *that) {
  // not called with keep here
  RegionNode *r = dynamic_cast<RegionNode *>(
      ctrl((new RegionNode({nullptr, ctrl(), that->ctrl()}))->keep()));

  Tomi::Vector<std::string> ns = reverseNames();
  // Note that we skip i==0, which is bound to '$ctrl'
  for (int i = 1; i < nIns(); i++) {
    if (in(i) != that->in(i)) { // No need for redundant Phis
      // If we are in lazy phi mode we need to a lookup
      // by name as it will trigger a phi creation
      Node *phi =
          new PhiNode(ns[i], {r, this->lookup(ns[i]), that->lookup(ns[i])});
      phi = phi->peephole();
      setDef(i, phi);
    }
  }
  that->kill();
  return r->unkeep()->peephole();
}
void ScopeNode::endLoop(ScopeNode *back, ScopeNode *exit) {
  Node *ctrl1 = ctrl();
  auto *loop = dynamic_cast<LoopNode *>(ctrl1);
  assert(loop && loop->inProgress());
  ctrl1->setDef(2, back->ctrl());
  for (int i = 1; i < nIns(); i++) {
    if (back->in(i) != this) {
      auto *phi = dynamic_cast<PhiNode *>(in(i));
      assert(phi->region() == ctrl1 && phi->in(2) == nullptr);
      phi->setDef(2, back->in(i));
    }
    if (exit->in(i) == this) // Replace a lazy-phi on the exit path also
      exit->setDef(i, in(i));
  }
  back->kill(); // Loop backedge is dead
  // Now one-time do a useless-phi removal
  for (int i = 1; i < nIns(); i++) {
    if (auto *phi = dynamic_cast<PhiNode *>(in(i))) {
      Node *in = phi->peephole();
      if (in != phi) {
        phi->subsume(in);
        setDef(i, in); // Set the Update back into Scope
      }
    }
  }
}
ScopeNode *ScopeNode::dup() { return dup(false); }
ScopeNode *ScopeNode::dup(bool loop) {
  auto *dup = new ScopeNode();
  // Our goals are:
  // 1) duplicate the name bindings of the ScopeNode across all stack levels
  // 2) Make the new ScopeNode a user of all the nodes bound
  // 3) Ensure that the order of defs is the same to allow easy merging
  for (const auto &syms : scopes) {
    dup->scopes.push_back(syms);
  }
  // Control comes first
  dup->addDef(ctrl());
  // now, all the inputs
  for (int i = 1; i < nIns(); i++) {
    // For lazy phis on loops we use a sentinel
    // that will trigger phi creation on update
    dup->addDef(loop ? this : in(i));
  }
  return dup;
}
Tomi::Vector<std::string> ScopeNode::reverseNames() {
  Tomi::Vector<std::string> names(nIns());
  for (const auto &syms : scopes) {
    for (const auto &pair : syms) {
      names[pair.second] = pair.first;
    }
  }
  return names;
}

std::string ScopeNode::CTRL = "$ctrl";
std::string ScopeNode::ARG0 = "arg";