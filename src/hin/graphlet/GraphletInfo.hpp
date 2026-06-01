#ifndef CATSCAN_TYPEISOMORPHISM_H
#define CATSCAN_TYPEISOMORPHISM_H

#include <string>
#include <tuple>
#include <vector>
#include <unordered_map>
#include "../HIN.hpp"


using namespace std;
using namespace CaTSCAN;


namespace CaTSCAN {


    typedef vector<TypeIdx> TypeVector;

    // Assigns a string of the form "(t0, t1, t2, ...)" to a graphlet count matching the type.
    // The number and order of the type depends on the respective graphlet.
    typedef unordered_map<string, Count> CountMap;  // maps type string of a graphlet to a count


    // Logs all relevant infos for a specific graphlet.
    struct GraphletInfo {
        Count total = 0;                        // total number of graphlets, independent of types
        Count totalCorrections = 0;             // total number of corrections (i.e. negative updates), useful for debugging.
        CountMap typedTotals;                   // logs graphlet counts for each type combination found
        CountMap typedCorrections;              // logs corrections for each type combination
        void update(const string& graphlet,     // Updates graphlet counts and corrections
                    const TypeVector& types,
                    Count count);
        void toFile(const string& path);
    };


    // logs all relevant info for 3-node graphlets
    struct ThreeNodeInfo {
        ThreeNodeInfo(Count nodes, Count edges);
        GraphletInfo wedges;
        GraphletInfo triangles;
        CountMap* trianglesPerNode{};     // number of typed triangles for each node, ordered by node index
        CountMap* trianglesPerEdge{};     // number of typed triangles for each edge, ordered by edge index
        unordered_map<string, EdgeIdx> edgeOfNodePair;        // stores EdgeIdx that connects a node pair
        void toFiles(const string& dir);
    };


    // logs all relevant info for 4-node graphlets
    struct FourNodeInfo {
        GraphletInfo threeStars;
        GraphletInfo threePaths;
        GraphletInfo tailedTriangles;
        GraphletInfo fourCycles;
        GraphletInfo diamonds;
        GraphletInfo fourCliques;
        void toFiles(const string& dir);
    };


    // logs all relevant info for 5-node graphlets
    struct FiveNodeInfo {
        GraphletInfo fourStars;
        GraphletInfo prongs;
        GraphletInfo fourPaths;
        GraphletInfo forkedTailedTriangles;
        GraphletInfo longTailedTriangles;
        GraphletInfo doubleTailedTriangles;
        GraphletInfo tailedFourCycles;
        GraphletInfo fiveCycles;
        GraphletInfo hourglasses;
        GraphletInfo cobras;
        GraphletInfo stingrays;
        GraphletInfo hattedFourCycles;
        GraphletInfo threeWedgeCollisions;
        GraphletInfo tailedFourCliques;
        GraphletInfo triangleStrips;
        GraphletInfo chordalWedgeCollisions;
        GraphletInfo wheels;
        GraphletInfo hattedFourCliques;
        GraphletInfo almostFiveCliques;
        GraphletInfo fiveCliques;
        GraphletInfo stellateTridents;
        void toFiles(const string& dir);
    };


    // logs all relevant info for graphlets of any size available
    struct CountInfo {
        CountInfo(Count nodes, Count edges);
        ThreeNodeInfo threeNode = ThreeNodeInfo(0, 0);
        FourNodeInfo fourNode;
        FiveNodeInfo fiveNode;
        void toFiles(const string& dir);
    };


    // Stores all triangles for each edge in CSR format
    // The number of triangles nTris is required for initialization
    struct TriangleList {
        TriangleList(CHIN *g, Count nTris);
        NodeIdx total;
        NodeIdx *triangles;
        EdgeIdx *trioffsets;
    };


    // This function makes sure all isomorphic graphlets are matched to the same type hash.
    // This requires in some cases that the types are passed in a specific order,
    // which is specified in the graphlet-specific iso functions.
    string iso(const TypeVector&, const string& graphlet);


    // converts a vector of types to a hash for the CountMap
    string prettyPrintHash(const TypeVector& types);


    // converts a pretty-printed type hash to a vector of types
    TypeVector decodeHash(const string& hash);


    // converts a type string for the trianglesPerNode in a ThreeNodeInfo to a vector of types
    TypeVector decodeNodeTriangle(const string& hash);


    // -------------------------------------------------------------------------------------------
    // -------------------- Detailed Iso Functions -----------------------------------------------
    // -------------------------------------------------------------------------------------------


    string isoClique(const TypeVector& types);

    string isoStar(const TypeVector& types);


    // -------------------- Three node graphlets -------------------------------------------------


    string isoWedge(const TypeVector& types);


    // -------------------- Four node graphlets --------------------------------------------------

    string isoThreePath(const TypeVector& types);


    string isoTailedTriangle(const TypeVector& types);


    string isoFourCycle(const TypeVector& types);


    string isoDiamond(const TypeVector& types);


    // -------------------- Five node graphlets --------------------------------------------------
    
    string isoFourPaths(const TypeVector& types);


    string isoProngs(const TypeVector& types);


    string isoForkedTailedTriangles(const TypeVector& types);


    string isoDoubleTailedTriangles(const TypeVector& types);


    string isoLongTailedTriangles(const TypeVector& types);


    string isoHourglass(const TypeVector& types);


    string isoStingray(const TypeVector& types);


    string isoStellateTrident(const TypeVector& types);


    string isoHattedFourCycle(const TypeVector& types);


    string isoTailedFourCycle(const TypeVector& types);


    string isoTailedFourClique(const TypeVector& types);


    string isoHattedFourClique(const TypeVector& types);


    string isoFiveCycle(const TypeVector& types);


    string isoThreeWedgeCollision(const TypeVector& types);


    string isoChordalWedgeCollision(const TypeVector& types);


    string isoWheel(const TypeVector& types);


    string isoTriStrip(const TypeVector& types);


    string isoAlmost5Clique(const TypeVector& types);


    string isoCobra(const TypeVector& types);


}   // namespace CaTSCAN


#endif // CATSCAN_TYPEISOMORPHISM_H
