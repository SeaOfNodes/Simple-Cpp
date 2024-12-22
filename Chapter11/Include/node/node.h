#ifndef NODE_H
#define NODE_H

#include <algorithm>
#include <cassert>
#include <sstream>

#include <bitset>
#include <functional>
#include <random>
#include <format>
#include <algorithm>

#include <type_traits>
#include "../../Include/tomi.h"
#include "../../Include/type/type.h"

// Custom hashing for Node:
class Node;
class CFGNode;

template<>
struct Tomi::hash<Node *> {
    unsigned long long operator()(Node *val);
};

/**
 * All Nodes in the Sea of Nodes IR inherit from the Node class.
 * The Node class provides common functionality used by all subtypes.
 * Subtypes of Node specialize by overriding methods.
/*
template <typename T>
typename std::vector<T>::Iterator find(std::vector<T> &vec, const T &value);

template <typename T>
void del(std::vector<T> &vec, typename std::vector<T>::iterator::pos);*/

class Node {
public:
    /**
     * Each node has a unique dense Node ID within a compilation context
     * The ID is useful for debugging, for using as an offset in a bitvector,
     * as well as for computing equality of nodes (to be implemented later).
     */
    int nid{};
    /**
     * Inputs to the node. These are use-def references to Nodes.
     * <p>
     * Generally fixed length, ordered, nulls allowed, no unused trailing space.
     * Ordering is required because e.g. "a/b" is different from "b/a".
     * The first input (offset 0) is often a {@link #isCFG} node.
     */
    Tomi::Vector<Node *> inputs;
    /**
     * Outputs reference Nodes that are not null and have this Node as an
     * input.  These nodes are users of this node, thus these are def-use
     * references to Nodes.
     * <p>
     * Outputs directly match inputs, making a directed graph that can be
     * walked in either direction.  These outputs are typically used for
     * efficient optimizations but otherwise have no semantics meaning.
     */
    Tomi::Vector<Node *> outputs;

    /**
     * Current computed type for this Node.  This value changes as the graph
     * changes and more knowledge is gained about the program
     */
    Type *type_ = nullptr;

    static int ITER_CNT;
    static int ITER_NOP_CNT;
    int hash_{};

    bool test_nid(int i);

    // Global Value Numbering.  Hash over opcode and inputs; hits in this table
    // are structurally equal.
    static Tomi::HashMap<Node *, Node *> GVN;

    // Hash of opcode and inputs
    unsigned long long hashCode();

    Tomi::Vector<Node *> deps_;


    virtual std::string err();
    CFGNode* cfg0();


    static int UID();

private:
    Tomi::Vector<bool> bitset;
    /**
     * A private Global Static mutable counter, for unique node id generation.
     * To make the compiler multithreaded, this field will have to move into a
     * TLS. Starting with value 1, to avoid bugs confusing node ID 0 with
     * uninitialized values.
     * */
    static int UNIQUE_ID;


public:
    Node() = default;

    Node(std::initializer_list<Node *> inputNodes);

    Node(Tomi::Vector<Node *> inputs);

    bool operator==(Node &);

    virtual ~Node() = default;

    // Easy reading label for debugger, e.g. "Add" or "Region" or "EQ"
    virtual std::string label() = 0;

    // Unique label for graph visualization, e.g. "Add12" or "Region30" or "EQ99"
    virtual std::string uniqueName();

    // Graphical label, e.g. "+" or "Region" or "=="
    virtual std::string glabel();

    std::string ToString();

    // ------------------------------------------------

    // Debugger printing.

    // This is a *deep* print.  This version will fail on cycles, which we will
    // correct later when we can parse programs with loops.  We print with a
    // tik-tok style; the common _print0 calls the per-Node _print1, which
    // calls back to _print0;
    std::ostringstream &print(std::ostringstream &b);

    // This is the common print: check for DEAD and print "DEAD" else call the
    // per-Node print1.
    virtual std::ostringstream &print_0(std::ostringstream &builder,
                                        Tomi::Vector<bool> &visited);

    // Every Node implements this.
    virtual std::ostringstream &print_1(std::ostringstream &builder,
                                        Tomi::Vector<bool> &) = 0;

    void printLine(std::ostringstream &builder);

    virtual int hash();

    virtual bool isMultiHead();

    virtual bool isMultiTail();

    // Graph Node & Edge manipulation
    [[nodiscard]] Node *in(std::size_t i) const;

    [[nodiscard]] Node *out(std::size_t i) const;

    [[nodiscard]] std::size_t nIns() const;

    [[nodiscard]] std::size_t nOuts() const;

    [[nodiscard]] bool isUnused() const;

    [[nodiscard]] virtual bool isCFG();

    [[nodiscard]] virtual bool isMem();

    /** Return block start from a isCFG() */
    [[nodiscard]] virtual Node* getBlockStart();


    /** Pinned in the schedule */
    [[nodiscard]] virtual bool isPinned();

    Node *addUse(Node *n);

    Node *setDef(int idx, Node *new_def);

    /**
     * Add a node to the list o dependencies. Only add it if its not
     * an input or output of this node, that is, it is at least one step
     * away. The node being added must benefit from this node being peepholed.
     */
    Node *addDep(Node *dep);

    Node *addDef(Node *new_def);

    // Remove the numbered input, compressing the inputs in-place. This
    // shuffles the order deterministically - which is suitable for Region and
    // Phi, but not for every Node.
    Node *delDef(int idx);

    bool delUse(Node *use);

    void popN(std::size_t n);

    // Kill a Node with no uses, by setting all of its defs to null.
    void kill();

    // Mostly used for asserts and printing.
    bool isDead();

    // Shortcuts to stop DCE mid-parse
    // Add bogus null use to keep node alive
    Node *keep();

    // Test "keep" status
    bool iskeep();

    Node *unkeep();

    // Replace self with nnn in the graph, making 'this' go dead
    void subsume(Node *nnn);

    // Graph-based optimisations

    static bool disablePeephole;
    // Try to peephole at this node and return a better replacement Node if
    // possible.

    Node *peephole();

    void unlock();

    // Unlike peephole above, this explicitly returns null for no-change, or
    // not-null for a better replacement
    Node *peepholeOpt();

    /*
     * Find a node by index.
     * */
    Node *deadCodeElim(Node *m);

    virtual Type *compute();

    // Set the type.  Assert monotonic progress.
    // If changing, add users to worklist.
    Type *setType(Type *type);

    virtual Node *idealize();

    virtual bool eq(Node *n);

    Node *swap12();

    // does this node contain all constants
    // Ignores i(0), as is usually control.
    virtual bool allCons(Node *dep);

    /**
     * Immediate dominator tree depth, used to approximate a real IDOM depth
     * during parsing where we do not have the whole program, and also
     * peepholes change the CFG incrementally.
     * <p>
     * See {@link <a href="https://en.wikipedia.org/wiki/Dominator_(graph_theory)">...</a>}
     */
    int idepth_{};    // IDOM depth approx; Zero is unset; non-zero is cached legit
    int _idepth(int idx);

    virtual int idepth();

    // Return the immediate dominator of this Node and compute dom tree depth.
    virtual Node *idom();

    virtual Node *copy(Node *lhs, Node *rhs);

    Node *find(Tomi::Vector<bool> visit, int nid_);

    // Move the dependents onto a worklist, and clear for future dependents.
    void moveDepsToWorkList();

    // Utility to walk the entire graph applying a function; return the first
    // not-null result.
    static Tomi::BitArray<10> WVISIT;

    template<typename T>
    T walk(const std::function<T(Node*)> &pred) {
//        assert(WVISIT.count() == 0);
        T rez = walk_(pred);
        WVISIT.reset();
        return rez;
    };

    template<typename T>
    T walk_(const std::function<T(Node*)> &pred) {
        if (WVISIT.test(nid)) {
            if constexpr (std::is_pointer<T>::value) {
                return nullptr;
            }
            return T();
        } // been there done that
        WVISIT.set(nid);
        // call pred here on each node
        T x = pred(this);
        if constexpr (std::is_pointer<T>::value) {
            if (x != nullptr) return x;
        } else {
            if (x != T()) return x;
        }

        for (auto *def: inputs) {
            if (def != nullptr) {
                T result = def->walk_(pred);
                if constexpr (std::is_pointer<T>::value) {
                    if (result != nullptr) return result;
                } else {
                    if (result != T()) return result;
                }
            }
        }

        for (Node *use: outputs) {
            if (use != nullptr) {
                T result = use->walk_(pred);
                if constexpr (std::is_pointer<T>::value) {
                    if (result != nullptr) return result;
                } else {
                    if (result != T()) return result;
                }
            }
        }
        if constexpr (std::is_pointer<T>::value) {
            return nullptr;
        }
        return T();
    };

    static void reset();

    std::string p(int depth);
};

class StopNode;

class IterPeeps {
    /*
     * * Classic WorkList, with a fast add/remove, dup removal, random pull.
     * The Node's nid is used to check membership in the worklist.
     */
public:
    static Node *add(Node *n);

    static void addAll(Tomi::Vector<Node *> ary);

    /**
     * Iterate peepholes to a fixed point
     */

    static StopNode *iterate(StopNode *stop, bool show);

    static bool MidAssert();

    static void reset();

    class WorkList {
    public:
        WorkList();

        WorkList(long seed);

        /* Useful stat - how many nodes are processed in the post parse iterative
         * opt */
        int totalWork;
        int trackWork; // track how many nodes are left to process
        // TOdo: fixed sized replace it with custom one
        Tomi::BitArray<100> on_;
        std::mt19937 rng; // For randomising pull from the WorkList
        Tomi::Vector<Node *> es;
        long seed{};

        /*
          Pushes a Node on the WorkList, ensuring no duplicates
          If Node is null it will not be added.
         */
        Node *push(Node *x);

        void addAll(Tomi::Vector<Node *>);

        /*
         * True if Node is on the WorkList
         */
        bool on(Node *x);

        /*
          Removes a random Node from the WorkList; null if WorkList is empty
         */
        Node *pop();

        void clear();
    };

    static WorkList WORK;

private:
    static bool MID_ASSERT;
    // static bool progressOnList(StopNode *stop);
};

#endif
