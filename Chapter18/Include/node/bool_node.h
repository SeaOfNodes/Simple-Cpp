#include "../../Include/node/node.h"

#include "../../Include/node/add_node.h"
#include "../../Include/node/constant_node.h"
#include "../../Include/type/integer_type.h"

#ifndef BOOL_NODE_H
#define BOOL_NODE_H

class BoolNode : public Node {
public:
    virtual std::string op();

    [[nodiscard]] virtual TypeInteger* doOp(TypeInteger* lhs, TypeInteger* rhs) const;
    [[nodiscard]] virtual bool doOp(double lhs, double rhs) const;

    Node* copyF(Node* lhs, Node* rhs);

    BoolNode(Node *lhs, Node *rhs);

    std::string label() override;

    std::string glabel() override;

    std::ostringstream &print_1(std::ostringstream &builder,
                                Tomi::Vector<bool>& visited) override;

    Type *compute() override;

    Node *idealize() override;
};

class EQ : public BoolNode {
public:
    EQ(Node *lhs, Node *rhs);

    Node *copy(Node *rhs, Node *lhs) override;

    std::string op() override;

    std::string label() override;

    TypeInteger* doOp(TypeInteger* lhs, TypeInteger* rhs) const override;
};

class LT : public BoolNode {
public:
    LT(Node *lhs, Node *rhs);

    Node *copy(Node *rhs, Node *lhs) override;

    std::string label() override;
    std::string glabel();
    std::string op() override;

    TypeInteger* doOp(TypeInteger* lhs, TypeInteger* rhs) const override;
};

class LE : public BoolNode {
public:
    LE(Node *lhs, Node *rhs);

    std::string glabel();
    Node *copy(Node *rhs, Node *lhs) override;

    std::string op() override;

    std::string label() override;

    TypeInteger* doOp(TypeInteger* lhs, TypeInteger* rhs) const override;
};
class EQF: public EQ {
public:
    EQF(Node *lhs, Node *rhs);
    [[nodiscard]] bool doOp(double lhs, double rhs) const override;
    Node* copy(Node*lhs, Node*rhs);
    std::string label() override;
};
class LTF: public LT {
public:
    LTF(Node *lhs, Node *rhs);
    [[nodiscard]]bool doOp(double lhs, double rhs) const override;
    Node* copy(Node*lhs, Node*rhs);
    std::string label() override;
};
class LEF: public LE {
public:
    LEF(Node *lhs, Node *rhs);
    [[nodiscard]] bool doOp(double lhs, double rhs) const override;
    Node* copy(Node*lhs, Node*rhs);
    std::string label() override;
};
#endif