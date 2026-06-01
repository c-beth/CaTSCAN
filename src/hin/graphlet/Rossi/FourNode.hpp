#ifndef CATSCAN_FOURNODE_HPP
#define CATSCAN_FOURNODE_HPP

#include "../../HIN.hpp"
#include "../GraphletInfo.hpp"
#include <unordered_set>


using std::unordered_set;


namespace Rossi {


    // Counts (by individual enumeration) all instances of 4-node graphlets for a single edge between nodes i and j.
    // Note: produces _induced_ counts
    void count4NodeForEdge(CHIN *cg,
                           NodeIdx v_i,
                           NodeIdx v_j,
                           unordered_set<NodeIdx> &S_i,     // stars centered around node v_i (produced by 3-node counter)
                           unordered_set<NodeIdx> &S_j,     // stars centered around node v_j (produced by 3-node counter)
                           unordered_set<NodeIdx> &T_ij,    // triangles between v_i and v_j (produced by 3-node counter)
                           FourNodeInfo &result);


    // Counts all instances of 3- and 4-node graphlets in the graph by individual enumeration.
    // This follows the approach by Rossi et al.in:
    // "Heterogeneous graphlets", ACM Transactions on Knowledge Discovery from Data (TKDD), 2020
    // Note: produces _induced_ counts
    CountInfo count3And4Node(CHIN *cg);


    // Corrects for over-counting due to counting for every edge in the graph and the combinatorics part.
    // Does not need to be called separately, as it is called in the main function (count3And4Node).
    void correctFourNodeCounts(FourNodeInfo &counts);


}   // namespace Rossi


#endif //CATSCAN_FOURNODE_HPP
