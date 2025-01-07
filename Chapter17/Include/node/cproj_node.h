#ifndef CPROJ_NODE_H
#define CPROJ_NODE_H
#include "../../Include/node/cfg_node.h"

class CProjNode: public CFGNode {
public:
     int idx_;
     std::string label_;
     CProjNode(Node* ctrl, int idx, std::string label);
     std::string label() override;
     std::ostringstream &print_1(std::ostringstream &builder, Tomi::Vector<bool>& visited) override;
     bool isMultiTail() override;
     bool blockHead() override;
     CFGNode* ctrl();
     Type* compute() override;
     Node* idealize() override;
     bool eq(Node* n) override;
     int hash() override;
};
#endif