#ifndef CATSCAN_FIVENODE_HPP
#define CATSCAN_FIVENODE_HPP

#include "../../HIN.hpp"
#include "../GraphletInfo.hpp"
#include "../ThreeNode/ThreeNode.hpp"
#include "../FourNode/FourNode.hpp"



using namespace CaTSCAN;


namespace CaTSCAN {

    // Calculates frequencies of all 5-node graphlets in an HIN.
    // Requires versions of the graph that contain all edges in nbors, as well as only the out-/in-oriented ones.
    // Also requires the 3-node graphlet frequencies.
    // Also requires the 4-node graphlet frequencies.
    FiveNodeInfo fiveNodeCalculator(CHIN* g,
                                    CHIN *gout,
                                    CHIN *gin,
                                    ThreeNodeInfo &threeNodeInfo,
                                    FourNodeInfo &fourNodeInfo);



    // ------------------------------------------------------------------------------------------
    // ---------------- Tree-based --------------------------------------------------------------
    // ------------------------------------------------------------------------------------------



    // Pattern 1: 4-star
    //
    //      1     2
    //       \   /
    //        \ /
    //         0
    //        / \
    //       /   \
    //      3     4
    //
    // Arguments:
    //   g: CSR of the graph
    //
    // Return:
    //   Total count of pattern 1 in the graph
    GraphletInfo countFourStars(CHIN *g);


    // Pattern 2: Prong
    //
    //         2
    //        /|\
    //       / | \
    //      1  3  4
    //      |
    //      |
    //      0
    //
    // Arguments:
    //   g: CSR of the graph
    //   fourNodeInfo: Counts + Info of 4-node graphlets
    //
    // Return:
    //   Total count of pattern 2 in the graph
    GraphletInfo countProngs(CHIN *gout, FourNodeInfo& fourNodeInfo);


    // Pattern 3: 4-Path
    //
    //
    //      0----1----2----3----4
    //
    // Arguments:
    //    g: CSR of the graph
    //    threeNodeInfo: Counts + Info of 3-node graphlets
    //    fourNodeInfo: Counts + Info of 4-node graphlets
    //
    // Return:
    //   Total count of pattern 3 in the graph
    GraphletInfo countFourPaths(CHIN *g, ThreeNodeInfo& threeNodeInfo, FourNodeInfo& fourNodeInfo);



    // ------------------------------------------------------------------------------------------
    // ---------------- Triangle-based ----------------------------------------------------------
    // ------------------------------------------------------------------------------------------



    // Pattern 4: Forked-Tailed-Triangle
    //
    //         2
    //        / \
    //       /   \
    //      0-----1
    //      |\
    //      | \
    //      3  4
    //
    // Arguments:
    //   g: CSR of the graph
    //   threeNodeInfo: Counts + Info of 3-node graphlets
    //
    // Return:
    //   Total count of pattern 4 in the graph
    GraphletInfo countForkedTailedTriangles(CHIN * gout, ThreeNodeInfo & threeNodeInfo);


    // Pattern 5: Long-Tailed-Triangle
    //
    //                   4
    //                  / \
    //                 /   \
    //      0----1----2-----3
    //
    // Arguments:
    //   g: CSR of the graph
    //   fourNodeInfo: Counts + Info of 4-node graphlets
    //   threeNodeInfo: Counts + Info of 3-node graphlets
    //
    // Return:
    //   Total count of pattern 5 in the graph
    GraphletInfo countLongTailedTriangles(CHIN *gout, FourNodeInfo& fourNodeInfo, ThreeNodeInfo& threeNodeInfo);


    // Pattern 6: Double-Tailed-Triangle
    //
    //         2
    //        / \
    //       /   \
    //      0-----1
    //      |     |
    //      |     |
    //      3     4
    //
    // Arguments:
    //   g: CSR of the graph
    //   gout: CSR (out-edges) of the graph
    //   fourNodeInfo: Counts + Info of 4-node graphlets
    //   threeNodeInfo: Counts + Info of 3-node graphlets
    //
    // Return:
    //   Total count of pattern 6 in the graph
    GraphletInfo countDoubleTailedTriangles(CHIN *g, CHIN *gout, FourNodeInfo& fourNodeInfo, ThreeNodeInfo& threeNodeInfo);


    // Pattern 9: Hourglass
    //
    //      1-----2
    //       \   /
    //        \ /
    //         0
    //        / \
    //       /   \
    //      3-----4
    //
    // Arguments:
    //   g: CSR of the graph
    //   fourNodeInfo: Counts + Info of 4-node graphlets
    //   threeNodeInfo: Counts + Info of 3-node graphlets
    //
    // Return:
    //   Total count of pattern 6 in the graph
    GraphletInfo countHourglasses(CHIN *gout, FourNodeInfo& fourNodeInfo, ThreeNodeInfo& threeNodeInfo);


    // Pattern 11: Stingray
    //
    //         3
    //        / \
    //       /   \
    //      2-----1----0
    //       \   /
    //        \ /
    //         4
    //
    // Arguments:
    //   g: CSR of the graph
    //   gout: CSR (out-edges) of the graph
    //   threeNodeInfo: Counts + Info of 3-node graphlets
    //
    // Return:
    //   Total count of pattern 11 in the graph
    GraphletInfo countStingrays(CHIN * g, CHIN * gout, ThreeNodeInfo & threeNodeInfo);


    // Pattern 14: Stellate Trident:
    //
    //               .0-.
    //              / |\ \
    //             2  | 3 4
    //              \ |/ /
    //               `1-´
    //
    // Arguments:
    //   gout: CSR (out-edges) of the graph
    //   threeNodeInfo: Counts + Info of 3-node graphlets
    //
    // Return:
    //   Total count of pattern 14 in the graph
    GraphletInfo countStellateTridents(CHIN * gout, ThreeNodeInfo & threeNodeInfo);


    // Pattern 10: Cobra:
    //
    //         2
    //        /| \
    //       / |  \
    //      4  |  1----0
    //       \ |  /
    //        \| /
    //         3
    //
    // Arguments:
    //   g: CSR of the graph
    //   gout: CSR (out-edges) of the graph
    //   TriangleList: List of triangle nodes for each edge
    //   fourNodeInfo: Counts + Info of 4-node graphlets
    //   threeNodeInfo: Counts + Info of 3-node graphlets
    //
    // Return:
    //   Total count of pattern 10 in the graph
    GraphletInfo countCobras(CHIN *g, CHIN *gout, TriangleList triList, FourNodeInfo& fourNodeInfo, ThreeNodeInfo& threeNodeInfo);



    // ------------------------------------------------------------------------------------------
    // ---------------- Clique-based ------------------------------------------------------------
    // ------------------------------------------------------------------------------------------



    // Pattern 7: Tailed-Four-Cycle
    //
    //      0
    //      |
    //      |
    //      1-----3
    //      |     |
    //      |     |
    //      2-----4
    //
    // Arguments:
    //   g: CSR  of the graph
    //   gout: CSR (out-edges) of the graph
    //   gin: CSR (in-edges) of the graph
    //   fourNodeInfo: Counts + Info of 4-node graphlets
    //
    // Return:
    //   Total count of pattern 7 in the graph
    GraphletInfo countTailedFourCycles(CHIN *g, CHIN *gout, CHIN *gin, FourNodeInfo& fourNodeInfo);


    // Pattern 12: Hatted-Four-Cycle
    //
    //         0
    //        / \
    //       /   \
    //      1-----3
    //      |     |
    //      |     |
    //      2-----4
    //
    // Arguments:
    //   g: CSR of the graph
    //   gout: CSR (out-edges) of the graph
    //   gin: CSR (in-edges) of the graph
    //   fourNodeInfo: Counts + Info of 4-node graphlets
    //   threeNodeInfo: Counts + Info of 3-node graphlets
    //
    //
    // Return:
    //   Total count of pattern 12 in the graph
    GraphletInfo countHattedFourCycles(CHIN *g, CHIN *gout, CHIN *gin, FourNodeInfo& fourNodeInfo, ThreeNodeInfo& goutInfo);


    // Pattern 15: Tailed-Four-Clique
    //
    //      0
    //      |
    //      |
    //      1----3
    //      | \/ |
    //      | /\ |
    //      2----4
    //
    // Arguments:
    //   g: CSR of the graph
    //   gout: CSR (out-edges) of the graph
    //
    // Return:
    //   Total count of pattern 15 in the graph
    GraphletInfo countTailedFourCliques(CHIN *g, CHIN *gout);


    // Pattern 19: Hatted-Four-Clique
    //
    //      3----2
    //      |\  /|\
    //      | \/ | 0
    //      | /\ |/
    //      4----1
    //
    // Arguments:
    //   gout: CSR (out-edges) of the graph
    //   threeNodeInfo: Counts + Info of 3-node graphlets
    //
    // Return:
    //   Total count of pattern 19 in the graph
    GraphletInfo countHattedFourCliques(CHIN *gout, ThreeNodeInfo& threeNodeInfo);


    // Pattern 21: Five-Clique
    //
    // Arguments:
    //   g: CSR of the graph
    //   gout: CSR (out-edges) of the graph
    //
    // Return:
    //   Total count of pattern 21 in the graph
    GraphletInfo countFiveCliques(CHIN *gout);


    // Pattern 8: Five Cycle
    //
    //      3----4
    //      |     \
    //      |      0
    //      |     /
    //      2----1
    //
    // Arguments:
    //   g: CSR of the graph
    //   gout: CSR (out-edges) of the graph
    //
    // Return:
    //   Total count of pattern 8 in the graph
    GraphletInfo countFiveCycles(CHIN *gout, CHIN *gin);


    // Pattern 17: Chordal-Wedge collision
    //
    //         0
    //        /  \
    //       /    \
    //      1      3
    //      | \  / |
    //      |  \/  |
    //      |  /\  |
    //      | /  \ |
    //      2------4
    //
    // Arguments:
    //   g: CSR of the graph
    //   TriangleList: List of triangle nodes for each edge
    //
    // Return:
    //   Total count of pattern 17 in the graph
    GraphletInfo countChordalWedgeCollisions(CHIN * g, TriangleList * triList);


    // Pattern 13: Three-Wedge collision
    //
    //                .0-.
    //               /  \ \
    //              2    3 4
    //               \  / /
    //                `1-´
    //
    // Arguments:
    //    g: CSR of the graph
    //
    // Return:
    //   Total count of pattern 13 in the graph
    GraphletInfo countThreeWedgeCollisions(CHIN * g);


    // Pattern 18: Wheel
    //
    //      3-----2
    //      | \ / |
    //      |  0  |
    //      | / \ |
    //      4-----1
    //
    // Arguments:
    //   g: CSR of the graph
    //   TriangleList: List of triangle nodes for each edge
    //
    // Return:
    //   Total count of pattern 18 in the graph
    GraphletInfo countWheels(CHIN *g, TriangleList *triList);


    // Pattern 16: Tri-Strip
    //
    //      3----0
    //      |   /|\
    //      |  / | 4
    //      | /  |/
    //      1----2
    //
    // Arguments:
    //   g: CSR of the graph
    //   fourNodeInfo: Counts + Info of 4-node graphlets
    //   threeNodeInfo: Counts + Info of 3-node graphlets
    //
    // Return:
    //   Total count of pattern 16 in the graph
    GraphletInfo countTriangleStrips(CHIN * g, FourNodeInfo & fourNodeInfo, ThreeNodeInfo & threeNodeInfo);


    // Pattern 20: Almost-five-Clique
    //
    // Arguments:
    //   g: CSR of the graph
    //
    // Return:
    //   Total count of pattern 20 in the graph
    GraphletInfo countAlmostFiveCliques(CHIN *g);


}   // namespace CaTSCAN


#endif //CATSCAN_FIVENODE_HPP
