#include "../Include/node/node.h"
#include "../Include/node/stop_node.h"

#include "../Include/tomi.h"
#include <algorithm>
#include <bitset>
#include <random>

class IterPeeps {
  /*
   * * Classic WorkList, with a fast add/remove, dup removal, random pull.
   * The Node's nid is used to check membership in the worklist.
   */
public:
  template <typename T> static T add(T n);

  static void addAll(Tomi::Vector<Node *> ary);
  /**
   * Iterate peepholes to a fixed point
   */
  static StopNode *iterate(StopNode *stop, bool show);
  static bool MidAssert;
  static bool midAssert();
  static bool progressOnList(Node *stop);
  static void reset();

  class WorkList {
  public:
    WorkList();
    WorkList(long seed);
    /* Useful stat - how many nodes are processed in the post parse iterative
     * opt */
    int totalWork;
    std::bitset<10> on_;
    std::mt19937 rng; // For randomising pull from the WorkList
    Tomi::Vector<Node *> es;
    long seed{};

    /*
      Pushes a Node on the WorkList, ensuring no duplicates
      If Node is null it will not be added.
     */
    template <typename T> T push(T x);

    template <typename T> void addAll(Tomi::Vector<T>);
    /*
     * True if Node is on the WorkList
     */
    template <typename T> bool on(T x);
    /*
      Removes a random Node from the WorkList; null if WorkList is empty
     */
    template <typename T> T pop();

    void clear();
  };
  static WorkList WORK;
};