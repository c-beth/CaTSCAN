#ifndef CATSCAN_THREENODE_HPP
#define CATSCAN_THREENODE_HPP

#include "../../HIN.hpp"
#include "../GraphletInfo.hpp"



using namespace std;
using namespace CaTSCAN;



namespace CaTSCAN {

    // Calculates frequencies of 3-node graphlets in a HIN.
    //
    // Wedges:
    //
    //  1    2
    //  |  /
    //  | /
    //  0
    //
    // Triangles:
    //
    //  1 -- 2
    //  |  /
    //  | /
    //  0
    //
    // Requires bot a version that contains all edges in nbors, as well as only the out-oriented ones.
    ThreeNodeInfo threeNodeEnumerator(CHIN *g, CHIN *gout);

}



#endif //CATSCAN_THREENODE_HPP
