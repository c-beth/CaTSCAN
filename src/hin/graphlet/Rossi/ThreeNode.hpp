#ifndef CATSCAN_THREENODE_HPP
#define CATSCAN_THREENODE_HPP

#include "../../HIN.hpp"
#include "../GraphletInfo.hpp"
#include <unordered_set>


namespace Rossi {


    // Counts (by individual enumeration) all instances of 3-node graphlets for a single edge between nodes i and j.
    // Note: produces _induced_ counts
    void count3NodeForEdge(CHIN *cg,
                           NodeIdx v_i,
                           NodeIdx v_j,
                           std::unordered_set<NodeIdx> &S_i,
                           std::unordered_set<NodeIdx> &S_j,
                           std::unordered_set<NodeIdx> &T_ij,
                           ThreeNodeInfo& result);


    // Counts all instances of 3-node graphlets in the graph by individual enumeration.
    // This follows the approach by Rossi et al.in:
    // "Heterogeneous graphlets", ACM Transactions on Knowledge Discovery from Data (TKDD), 2020
    // Note: produces _induced_ counts
    ThreeNodeInfo count3Node(CHIN *cg);


    // Corrects for over-counting due to counting for every edge in the graph.
    // Does not need to be called separately, as it is called in the main function (count3Node or count3And4Node).
    void correctThreeNodeCounts(ThreeNodeInfo &counts);


}   // namespace Rossi


#endif //CATSCAN_THREENODE_HPP
