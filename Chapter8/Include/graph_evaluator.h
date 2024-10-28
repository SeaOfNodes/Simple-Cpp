#include <vector>
#include <unordered_map>

#include "../Include/node/start_node.h"
#include "../Include/node/constant_node.h"
#include "../Include/node/add_node.h"
#include "../Include/node/bool_node.h"

#include "../Include/utils.h"
#include "../Include/node/proj_node.h"
#include "../Include/node/divnode.h"

class GraphEvaluator {
public:
  enum class ResultType {VALUE, FALLTHROUGH, TIMEOUT};
  struct result {
    ResultType type;
    long value;
  };
  std::vector<bool> bitset;

  /**
     * Find the start node from some node in the graph or null if there is no start node
   */
   static StartNode* findStart(std::vector<bool>& visit, Node* node);
   /**
     * Find the control output from a control node
    */
   static Node* findControl(Node* control);
   /**
     * Find the projection for a node
    */
   static ProjNode* findProjection(Node* node, int idx);
   /**
     * Cache values for phi and parameter projection nodes.
    */

   std::unordered_map<Node*, int64_t> cacheValues;
   /**
     * Cache for loop phis as they can depend on itself or other loop phis
    */
   static long evaluate(Node* graph);
   static long evaluate(Node* graph, long parameter);
   static long evaluate(Node* graph, long parameter, int loops);

   static result evaluateWithResult(Node* graph, long parameter, int loops);
 private:
   std::vector<int64_t> loopPhiCache;
   long div(DivNode* div);
   /**
     * Calculate the value of a node
    */
   long getValue(Node* node);

   /**
     * Special case of latchPhis when phis can depend on phis of the same region.
    */
   void latchLoopPhis(RegionNode* region, Node* prev);

   /**
     * Calculate the values of phis of the region and caches the values. The phis are not allowed to depend on other phis of the region.
    */
    void latchPhis(RegionNode* region, Node* prev);
    /**
     * Run the graph until either a return is found or the number of loop iterations are done.
     */
    result evaluate(StartNode* start, long parameter, int loops);


};
