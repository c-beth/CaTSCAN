#include "FiveNode.hpp"


// Pattern 16: Tri-Strip
//
//      3----0
//      |   /|\
//      |  / | 4
//      | /  |/
//      1----2
//
// Method:
//   Go over each triangle instance 0,1,2 three times.
//   For every time count hats on the edges that start from node v_i.
//   Apply corrections for when 3,4 are identical (4-clique).
//
// Arguments:
//   g: CSR of the graph
//   fourNodeInfo: Counts + Info of 4-node graphlets
//   threeNodeInfo: Counts + Info of 3-node graphlets
//
// Return:
//   Total count of pattern 16 in the graph

GraphletInfo CaTSCAN::countTriangleStrips(CHIN *g,
                                          FourNodeInfo& fourNodeInfo,
                                          ThreeNodeInfo& threeNodeInfo) {

    GraphletInfo ret;
    Count count;

    for (NodeIdx v_i = 0; v_i < g->nNodes; v_i++) {
        TypeIdx t_i = g->nodeTypes[v_i];

        // enumerate triangles as cut set
        for (EdgeIdx e_ij = g->offsets[v_i]; e_ij < g->offsets[v_i + 1]; e_ij++) {   // loop over neighbor of v_i
            NodeIdx v_j = g->nbors[e_ij];
            TypeIdx t_j = g->nodeTypes[v_j];
            EdgeIdx e_out;
            if (v_j < v_i)
                e_out = threeNodeInfo.edgeOfNodePair["(" + to_string(v_j) + "," + to_string(v_i) + ")"];
            else
                e_out = threeNodeInfo.edgeOfNodePair["(" + to_string(v_i) + "," + to_string(v_j) + ")"];
            CountMap &tris_e_ij = threeNodeInfo.trianglesPerEdge[e_out];

            for (EdgeIdx e_ik = e_ij + 1; e_ik < g->offsets[v_i + 1]; e_ik++) {      // loop over another neighbor of v_i
                NodeIdx v_k = g->nbors[e_ik];
                if (g->getEdge(v_j, v_k) == invalidEdge)
                    continue;
                TypeIdx t_k = g->nodeTypes[v_k];
                EdgeIdx e2_out;
                if (v_k < v_i)
                    e2_out = threeNodeInfo.edgeOfNodePair["(" + to_string(v_k) + "," + to_string(v_i) + ")"];
                else
                    e2_out = threeNodeInfo.edgeOfNodePair["(" + to_string(v_i) + "," + to_string(v_k) + ")"];
                CountMap &tris_e_ik = threeNodeInfo.trianglesPerEdge[e2_out];

                // with the triangle as a cut set, combine with adjacent triangles to form a strip
                for (auto & tri: tris_e_ij) {
                    TypeIdx tip1_type = decodeNodeTriangle(tri.first)[0];
                    Count triangle1_count = tri.second;
                    if (tip1_type == t_k)
                        triangle1_count--;
                    if (triangle1_count <= 0)
                        continue;

                    for (auto & tri2: tris_e_ik) {
                        TypeIdx tip2_type = decodeNodeTriangle(tri2.first)[0];
                        Count triangle2_count = tri2.second;
                        if (tip2_type == t_j)
                            triangle2_count--;
                        if (triangle2_count <= 0)
                            continue;
                        count = triangle1_count * triangle2_count;
                        TypeVector types = {t_i, t_j, t_k, tip1_type, tip2_type};
                        ret.update("tri-strip", types, count);
                    }
                }
            }
        }
    }

    // Apply correction for over-counting
    CountMap &FourCliques = fourNodeInfo.fourCliques.typedTotals;
    for (auto const& [type_hash, fourClique_count] :  FourCliques) {
        TypeVector t = decodeHash(type_hash);

        TypeVector TS_types_1 = {t[0], t[1], t[2], t[3], t[3]};
        TypeVector TS_types_2 = {t[1], t[0], t[2], t[3], t[3]};
        TypeVector TS_types_3 = {t[2], t[0], t[1], t[3], t[3]};

        TypeVector TS_types_4 = {t[0], t[1], t[3], t[2], t[2]};
        TypeVector TS_types_5 = {t[1], t[0], t[3], t[2], t[2]};
        TypeVector TS_types_6 = {t[3], t[0], t[1], t[2], t[2]};

        TypeVector TS_types_7 = {t[0], t[2], t[3], t[1], t[1]};
        TypeVector TS_types_8 = {t[2], t[0], t[3], t[1], t[1]};
        TypeVector TS_types_9 = {t[3], t[0], t[2], t[1], t[1]};

        TypeVector TS_types_10 = {t[1], t[2], t[3], t[0], t[0]};
        TypeVector TS_types_11 = {t[2], t[1], t[3], t[0], t[0]};
        TypeVector TS_types_12 = {t[3], t[1], t[2], t[0], t[0]};

        ret.update("tri-strip",TS_types_1,-fourClique_count);
        ret.update("tri-strip",TS_types_2,-fourClique_count);
        ret.update("tri-strip",TS_types_3,-fourClique_count);
        ret.update("tri-strip",TS_types_4,-fourClique_count);
        ret.update("tri-strip",TS_types_5,-fourClique_count);
        ret.update("tri-strip",TS_types_6,-fourClique_count);
        ret.update("tri-strip",TS_types_7,-fourClique_count);
        ret.update("tri-strip",TS_types_8,-fourClique_count);
        ret.update("tri-strip",TS_types_9,-fourClique_count);
        ret.update("tri-strip",TS_types_10,-fourClique_count);
        ret.update("tri-strip",TS_types_11,-fourClique_count);
        ret.update("tri-strip",TS_types_12,-fourClique_count);
    }
    return ret;
}
