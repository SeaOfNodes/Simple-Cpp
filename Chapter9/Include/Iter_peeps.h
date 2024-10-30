#include "../Include/node/node.h"
#include "../Include/node/stop_node.h"

#include "../Include/tomi.h"


#include <random>
#include <algorithm>

class IterPeeps {
  /*
   * * Classic WorkList, with a fast add/remove, dup removal, random pull.
   * The Node's nid is used to check membership in the worklist.
   */
public:
  template <typename T>
  static T add(T n);
  static void addAll(Tomi::Vector<Node*> ary);
  /**
     * Iterate peepholes to a fixed point
   */
  static StopNode* iterate(StopNode* stop, bool show);
  static bool MidAssert;
  static bool midAssert();
  static bool progressOnList(Node* stop);
  static void reset();

  WorkList<Node*> WORK;
  template <typename T> class WorkList {
  private:
    Tomi::Vector<Node *> es_;
    int len_;
    Tomi::Vector<bool> onV; // Bitset if Node._nid is on WorkList
    std::mt19937 rng_;      // For randomizing pull from the WorkList
    long seed;
    /* Useful stat - how many nodes are processed in the post parse iterative
     * opt */
    long totalWork;
    WorkList();
    WorkList(long seed);

  public:
    /*
      Pushes a Node on the WorkList, ensuring no duplicates
      If Node is null it will not be added.
     */
    T push(T x);
    void addAll(Tomi::Vector<T>);
    /*
     * True if Node is on the WorkList
     */
    bool on(T x);
    /*
      Removes a random Node from the WorkList; null if WorkList is empty
     */
    T pop();

    void clear();
  };
};