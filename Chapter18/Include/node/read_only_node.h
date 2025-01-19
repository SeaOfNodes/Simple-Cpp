#ifndef read_only_node_h
#define read_only_node_h

#include "node.h"

class ReadOnlyNode: public Node {
public:
    explicit ReadOnlyNode(Node*n );
        std::string label() override;

    std::ostringstream &print_1(std::ostringstream &builder,
                                Tomi::Vector<bool> &) override;

    Type* compute() override;

    Node* idealize() override;
};

#endif