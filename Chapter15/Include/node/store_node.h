#ifndef STORE_NODE_H
#define STORE_NODE_H

/**
 * Store represents setting a value to a memory based object, in chapter 10
 * this means a field inside a struct.
 */

#include "../../Include/node/mem_op_node.h"
 class StoreNode: public MemOpNode {
 public:
     bool init; // Initializing writes are allowed to write null
     StoreNode() = default;
     StoreNode(std::string name, int alias, Type* glb, Node*ctrl, Node* memSlice, Node* memPtr, Node* value, bool init);
     std::string label() override;
     std::string glabel() override;

     bool isMem();
     Node* val();

     std::ostringstream &print_1(std::ostringstream &builder, Tomi::Vector<bool>& visited);
     Type* compute() override;
     Node* idealize() override;
     bool checkNoUseBeyond(Node* that);
     std::string err() override;

 };
#endif