#include "FourNode.hpp"


FourNodeInfo CaTSCAN::fourNodeCalculator(CHIN *g, CHIN *gout, CHIN *gin, ThreeNodeInfo &threeNodeInfo) {
    FourNodeInfo ret;
    ret.threeStars = countThreeStars(g);
    ret.threePaths = countThreePaths(g, gout, threeNodeInfo);
    ret.tailedTriangles = countTailedTriangles(g, gout, threeNodeInfo);
    ret.fourCycles = countFourCycles(gout, gin);
    ret.diamonds = countDiamonds(gout, threeNodeInfo);
    ret.fourCliques = countFourCliques(gout);
    return ret;
}
