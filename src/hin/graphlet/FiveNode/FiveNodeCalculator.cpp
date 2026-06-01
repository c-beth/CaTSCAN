#include "FiveNode.hpp"


FiveNodeInfo CaTSCAN::fiveNodeCalculator(CHIN *g,
                                         CHIN *gout,
                                         CHIN* gin,
                                         ThreeNodeInfo& threeNodeInfo,
                                         FourNodeInfo& fourNodeInfo) {

    FiveNodeInfo ret;
    TriangleList triList = TriangleList(g, threeNodeInfo.triangles.total);

    // Tree-based
    ret.fourStars = countFourStars(g);
    ret.prongs = countProngs(g, fourNodeInfo);
    ret.fourPaths = countFourPaths(g, threeNodeInfo, fourNodeInfo);

    // Triangle-based
    ret.forkedTailedTriangles = countForkedTailedTriangles(g, threeNodeInfo);
    ret.doubleTailedTriangles = countDoubleTailedTriangles(g, gout, fourNodeInfo, threeNodeInfo);
    ret.longTailedTriangles = countLongTailedTriangles(g, fourNodeInfo, threeNodeInfo);
    ret.hourglasses = countHourglasses(g, fourNodeInfo, threeNodeInfo);
    ret.stingrays = countStingrays(g, gout, threeNodeInfo );
    ret.stellateTridents = countStellateTridents(gout, threeNodeInfo);
    ret.cobras = countCobras(g, gout, triList, fourNodeInfo, threeNodeInfo);

    // Clique-based
    ret.hattedFourCycles = countHattedFourCycles(g, gout, gin, fourNodeInfo, threeNodeInfo);
    ret.tailedFourCycles = countTailedFourCycles(g, gout, gin, fourNodeInfo);
    ret.tailedFourCliques = countTailedFourCliques(g, gout);
    ret.hattedFourCliques = countHattedFourCliques(gout,threeNodeInfo);
    ret.fiveCliques = countFiveCliques(gout);
    ret.fiveCycles = countFiveCycles(gout, gin);
    ret.chordalWedgeCollisions = countChordalWedgeCollisions(g, &triList);
    ret.threeWedgeCollisions = countThreeWedgeCollisions(g);
    ret.wheels = countWheels(g, &triList);
    ret.triangleStrips = countTriangleStrips(g, fourNodeInfo, threeNodeInfo);
    ret.almostFiveCliques = countAlmostFiveCliques(g);

    return ret;
}
