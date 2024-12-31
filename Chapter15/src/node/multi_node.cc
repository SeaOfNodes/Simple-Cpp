#include "../../Include/node/multi_node.h"
#include "../../Include/node/proj_node.h"
#include "../../Include/node/cproj_node.h"
MultiNode::MultiNode(std::initializer_list<Node *> nodes) : CFGNode(nodes) {}
MultiNode::MultiNode(Tomi::Vector<Node*> nodes): CFGNode(nodes) {}
std::string MultiNode::label() { return "MultiNode"; }

std::ostringstream &MultiNode::print_1(std::ostringstream &builder,
                                       Tomi::Vector<bool> &visited) {
    return builder;
}

ProjNode *MultiNode::proj(int idx) {
    for (Node *out: outputs) {
        if (auto *prj = dynamic_cast<ProjNode *>(out); prj->idx_ == idx) {
            return prj;
        }
    }
    return nullptr;
}

CProjNode *MultiNode::cproj(int idx) {
    for (Node *out: outputs) {
        auto *prj = dynamic_cast<CProjNode *>(out);
        if (prj && prj->idx_ == idx) {
            return prj;
        }
    }
    return nullptr;
}