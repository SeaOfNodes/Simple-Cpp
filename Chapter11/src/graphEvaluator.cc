#include "../Include/graph_evaluator.h"
#include "../Include/node/minus_node.h"
#include "../Include/node/sub_node.h"
#include "../Include/node/region_node.h"
#include "../Include/node/return_node.h"

StartNode *GraphEvaluator::findStart(Tomi::Vector<bool> &visit, Node *node) {
  if (node == nullptr)
    return nullptr;

  if (node->nid >= visit.size()) {
    visit.resize(node->nid + 1, false);
  }

  if (visit[node->nid])
    return nullptr;

  visit[node->nid] = true;

  auto*start = dynamic_cast<StartNode*>(node) ? dynamic_cast<StartNode*>(node) : nullptr;

  for (Node *def : node->inputs) {
    auto res = findStart(visit, def);
    if (res != nullptr) {
        start = res;
    }
  }
  for (Node *use : node->outputs) {
    auto res = findStart(visit, use);
    if (res != nullptr)
      start = res;
  }
  return start;
}

Node *GraphEvaluator::findControl(Node *control) {
  for (Node *use : control->outputs) {
    if (use->isCFG())
      return use;
  }
  return nullptr;
}

ProjNode *GraphEvaluator::findProjection(Node *node, int idx) {
  for (Node *use : node->outputs) {
    if (auto *proj = dynamic_cast<ProjNode *>(use); proj && proj->idx_ == idx)
      return proj;
  }
  return nullptr;
}

long GraphEvaluator::div(DivNode *div) {
  long in2 = getValue(div->in(2));
  return in2 == 0 ? 0 : getValue(div->in(1)) / in2;
}

long GraphEvaluator::getValue(Node *node) {
  auto cache = cacheValues.get(node);
  if (cache != nullptr)
    return *cache;
  // Determine the type of `node` and compute the result accordingly
  if (auto *cons = dynamic_cast<ConstantNode *>(node)) {
    int64_t value = dynamic_cast<TypeInteger *>(cons->compute())->value();
    return value;
  } else if (auto *add = dynamic_cast<AddNode *>(node)) {
    int64_t value = getValue(add->in(1)) + getValue(add->in(2));
    return value;
  } else if (auto *eq = dynamic_cast<EQ *>(node)) {
    int64_t value = getValue(eq->in(1)) == getValue(eq->in(2)) ? 1 : 0;
    return value;
  } else if (auto *le = dynamic_cast<LE *>(node)) {
    int64_t value = getValue(le->in(1)) <= getValue(le->in(2)) ? 1 : 0;
    return value;
  } else if (auto *lt = dynamic_cast<LT *>(node)) {
    int64_t value = getValue(lt->in(1)) < getValue(lt->in(2)) ? 1 : 0;
    return value;
  } else if (auto *div1 = dynamic_cast<DivNode *>(node)) {
    int64_t value = div(div1); // Assumes `div` is a function taking a DivNode*
    return value;
  } else if (auto *minus = dynamic_cast<MinusNode *>(node)) {
    int64_t value = -getValue(minus->in(1));
    return value;
  } else if (auto *mul = dynamic_cast<MulNode *>(node)) {
    int64_t value = getValue(mul->in(1)) * getValue(mul->in(2));
    return value;
  } else if (auto *notNode = dynamic_cast<NotNode *>(node)) {
    int64_t value = getValue(notNode->in(1)) == 0 ? 1 : 0;
    return value;
  } else if (auto *sub = dynamic_cast<SubNode *>(node)) {
    int64_t value = getValue(sub->in(1)) - getValue(sub->in(2));
    return value;
  } else {
    throw std::runtime_error("Unexpected node type");
  }
}

void GraphEvaluator::latchLoopPhis(RegionNode *region, Node *prev) {
  auto idx_it = std::find(region->inputs.begin(), region->inputs.end(), prev);
  // Calculate the integer index
  int idx = static_cast<int>(std::distance(region->inputs.begin(), idx_it));
  assert(idx > 0);
  int i = 0;
  for (Node *use : region->outputs) {
    if (auto *phi = dynamic_cast<PhiNode *>(use); phi) {
      auto value = getValue(phi->in(idx));
      if (i == loopPhiCache.size()) {
        loopPhiCache.resize(loopPhiCache.size() * 2);
      }
      loopPhiCache[i++] = value;
    }
  }
  i = 0;
  for (Node *use : region->outputs) {
    if (auto *phi = dynamic_cast<PhiNode *>(use); phi)
      cacheValues.put(phi, loopPhiCache[i++]);
  }
}

void GraphEvaluator::latchPhis(RegionNode *region, Node *prev) {
  auto idx_it = std::find(region->inputs.begin(), region->inputs.end(), prev);
  // Calculate the integer index
  int idx = static_cast<int>(std::distance(region->inputs.begin(), idx_it));
  for (Node *use : region->outputs) {
    if (auto *phi = dynamic_cast<PhiNode *>(use); phi) {
      auto value = getValue(phi->in(idx));
      cacheValues.put(phi, value);
    }
  }
}

GraphEvaluator::result GraphEvaluator::evaluate(StartNode *start,
                                                long parameter, int loops) {
  // get the arg from startnode
  Node *parameter1 = findProjection(start, 1);
  if (parameter1)
    cacheValues.put(parameter1, parameter);
  ;
  std::ostringstream builder;
  // get the control from startnode
  Node *control = findProjection(start, 0);
  Node *prev = start;
  while (control != nullptr) {
    Node *next;
    if (auto *region = dynamic_cast<RegionNode *>(control)) {
      if (dynamic_cast<LoopNode *>(region) && region->in(1) != prev) {
        if (loops-- <= 0)
          return {ResultType::TIMEOUT, 0};
        latchLoopPhis(region, prev);
      } else {
        latchPhis(region, prev);
      }
      next = findControl(region);
    } else if (auto *cond = dynamic_cast<IfNode *>(control)) {
      next = findProjection(cond, getValue(cond->in(1)) != 0 ? 0 : 1);
    } else if (auto *ret = dynamic_cast<ReturnNode *>(control)) {
      return {ResultType::VALUE, getValue(ret->in(1))};
    } else if (auto *proj = dynamic_cast<ProjNode *>(control)) {
      next = findControl(control);
    } else {
      throw std::runtime_error("Unexpected control node"); // Error handling
    }
    prev = control;
    control = next;
  }
  return {ResultType::FALLTHROUGH, 0};
}

long GraphEvaluator::evaluate(Node *graph) { return evaluate(graph, 0); }

long GraphEvaluator::evaluate(Node *graph, long parameter) {
  return evaluate(graph, parameter, 1000);
}

long GraphEvaluator::evaluate(Node *graph, long parameter, int loops) {
  auto res = evaluateWithResult(graph, parameter, loops);
  if (res.type == ResultType::TIMEOUT)
    throw std::runtime_error("TimeOut");
  return res.value;
}

GraphEvaluator::result
GraphEvaluator::evaluateWithResult(Node *graph, long parameter, int loops) {
  Tomi::Vector<bool> bitset;
  auto start = findStart(bitset, graph);
  if (start == nullptr) {
    result res{};
    res.type = ResultType::TIMEOUT;
    res.value = 0;
    return res;
  }

  auto *evaluator = alloc.new_object<GraphEvaluator>();
  evaluator->loopPhiCache.reserve(16);
  return evaluator->evaluate(start, parameter, loops);
}
