#include "ThreeNode.hpp"
#include <string>


using namespace std;


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

ThreeNodeInfo CaTSCAN::threeNodeEnumerator(CHIN *g, CHIN *gout) {
    ThreeNodeInfo ret = ThreeNodeInfo(g->nNodes, g->nEdges);
    TypeVector types;

    for (NodeIdx v_i = 0; v_i < gout->nNodes; v_i++) {      // loop over vertices
        TypeIdx t_i = gout->nodeTypes[v_i];

        // enumerate triangles
        for (EdgeIdx e_ij = gout->offsets[v_i]; e_ij < gout->offsets[v_i + 1]; e_ij++) {   // loop over neighbor of v_i
            NodeIdx v_j = gout->nbors[e_ij];
            TypeIdx t_j = gout->nodeTypes[v_j];

            for (EdgeIdx e_ik = e_ij + 1; e_ik < gout->offsets[v_i + 1]; e_ik++) {      // loop over another neighbor of v_i
                NodeIdx v_k = gout->nbors[e_ik];
                TypeIdx t_k = gout->nodeTypes[v_k];
                // we are now looking at wedge (v_i, v_j, v_k), centered at v_i

                // need to check both directions, since nbors is grouped by typing and not strictly sorted by degree
                EdgeIdx e_jk = gout->getEdge(v_j, v_k);
                EdgeIdx e_kj = gout->getEdge(v_k, v_j);
                if (e_jk != invalidEdge or e_kj != invalidEdge) {        // edge  between v_j, v_k is present

                    types = {t_i, t_j, t_k};
                    ret.triangles.update("triangle", types, 1);

                    // update per-vertex counts
                    ret.trianglesPerNode[v_i][to_string(min(t_j, t_k)) + '-' + to_string(max(t_j, t_k))]++;
                    ret.trianglesPerNode[v_j][to_string(min(t_i, t_k)) + '-' + to_string(max(t_i, t_k))]++;
                    ret.trianglesPerNode[v_k][to_string(min(t_i, t_j)) + '-' + to_string(max(t_i, t_j))]++;

                    // update per-edge counts. Note that location used is same as position in g->nbors
                    ret.trianglesPerEdge[e_ij][to_string(t_k)]++;
                    ret.trianglesPerEdge[e_ik][to_string(t_j)]++;
                    if (e_jk != invalidEdge) {
                        ret.trianglesPerEdge[e_jk][to_string(t_i)]++;
                        ret.edgeOfNodePair["(" + to_string(v_j) + "," + to_string(v_k) + ")" ] = e_jk;
                    }
                    if (e_kj != invalidEdge) {
                        ret.trianglesPerEdge[e_kj][to_string(t_i)]++;
                        ret.edgeOfNodePair["(" + to_string(v_k) + "," + to_string(v_j) + ")" ] = e_kj;
                    }
                    ret.edgeOfNodePair["(" + to_string(v_i) + "," + to_string(v_j) + ")" ] = e_ij;
                    ret.edgeOfNodePair["(" + to_string(v_i) + "," + to_string(v_k) + ")" ] = e_ik ;

                }
            }
        }

        // enumerate node types to derive number of wedges with combinatorics (use g instead of gout)
        Count wedgeCount;
        for (TypeIdx t_1 = 0; t_1 < g->nTypes; t_1++) {
            Count d_1 = g->type_degree(v_i, t_1);
            if (d_1 == 0)
                continue;
            wedgeCount = d_1 * (d_1 - 1) / 2;    // d_1 choose 2
            types = {t_i, t_1, t_1};
            ret.wedges.update("wedge", types, wedgeCount);

            for (TypeIdx t_2 = t_1 + 1; t_2 < g->nTypes; t_2++) {
                Count d_2 = g->type_degree(v_i, t_2);
                if (d_2 == 0)
                    continue;
                wedgeCount = d_1 * d_2;    // d_1 choose 1 * d_2 choose 1
                types = {t_i, t_1, t_2};
                ret.wedges.update("wedge", types, wedgeCount);
            }
        }
    }
    return ret;
}
