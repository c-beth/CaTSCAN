#ifndef CATSCAN_FOURNODE_HPP
#define CATSCAN_FOURNODE_HPP

#include "../../HIN.hpp"
#include "../GraphletInfo.hpp"
#include "../ThreeNode/ThreeNode.hpp"



using namespace CaTSCAN;



namespace CaTSCAN {


    // Calculates frequencies of all 4-node graphlets in an HIN.
    // Requires versions of the graph that contain all edges in nbors, as well as only the out-/in-oriented ones.
    // Also requires the 3-node graphlet frequencies.
    FourNodeInfo fourNodeCalculator(CHIN *g, CHIN *gout, CHIN *gin, ThreeNodeInfo& threeNodeInfo);


    // Calculates frequencies of 3-stars in an HIN.
    //
    //  1    2
    //  |  /
    //  | /
    //  0----3
    GraphletInfo countThreeStars(CHIN *g);


    // Calculates frequencies of 4-paths in an HIN.
    // Requires two versions of the graph that contain all edges in nbors, as well as only the out-oriented ones.
    // Also requires the 3-node graphlet frequencies.
    //
    //  0    3
    //  |    |
    //  |    |
    //  1----2
    //
    GraphletInfo countThreePaths(CHIN *g, CHIN *gout, ThreeNodeInfo& threeNodeInfo);


    // Calculates frequencies of tailed triangles in an HIN.
    // Requires two versions of the graph that contain all out-edges in nbors, as well as on with the in-oriented ones.
    // Also requires the 3-node graphlet frequencies.
    //
    //  1    2
    //  |  / |
    //  | /  |
    //  0----3
    //
    GraphletInfo countTailedTriangles(CHIN *g, CHIN *gout, ThreeNodeInfo& threeNodeInfo);


    // Calculates frequencies of 4-cycles in an HIN.
    // Requires versions of the graph that contain all edges in nbors, as well as only the out-/in-oriented ones.
    //
    //  1----2
    //  |    |
    //  |    |
    //  0----3
    //
    GraphletInfo countFourCycles(CHIN *gout, CHIN *gin);


    // Calculates frequencies of diamonds in an HIN.
    // Requires only the version of the graph, where nbors contains only out-neighbors.
    // Also requires the 3-node graphlet frequencies.
    //
    //  3----1
    //  |  / |
    //  | /  |
    //  0----2
    //
    GraphletInfo countDiamonds(CHIN *gout, ThreeNodeInfo& threeNodeInfo);


    // Calculates frequencies of 4-cliques in an HIN.
    // Requires only the version of the graph, where nbors contains only out-neighbors.
    //
    //  1----2
    //  | \/ |
    //  | /\ |
    //  0----3
    //
    GraphletInfo countFourCliques(CHIN *gout);


}


#endif //CATSCAN_FOURNODE_HPP
