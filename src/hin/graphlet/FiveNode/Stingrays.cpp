#include "FiveNode.hpp"


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
// Method:
//   Loop over all edges of the graph as 1-2.
//   Choose two distinct triangles that edge 1-2 participates in.
//   Choose one neighbor of node 1 and combine with the two triangles.
//
// Arguments:
//   g: CSR of the graph
//   gout: CSR (out-edges) of the graph
//   threeNodeInfo: Counts + Info of 3-node graphlets
//
// Return:
//   Total count of pattern 11 in the graph

GraphletInfo CaTSCAN::countStingrays(CHIN *g,
                                     CHIN *gout,
                                     ThreeNodeInfo& threeNodeInfo ) {

    GraphletInfo ret;
    Count count;
    TypeVector types;

    for (NodeIdx v_i = 0; v_i < g->nNodes; v_i++) {
        TypeIdx t_i = g->nodeTypes[v_i];
        for (EdgeIdx e_ij = gout->offsets[v_i]; e_ij < gout->offsets[v_i + 1]; e_ij++) {
            NodeIdx v_j = gout->nbors[e_ij];
            TypeIdx t_j = g->nodeTypes[v_j];
            CountMap &tris_e_ij = threeNodeInfo.trianglesPerEdge[e_ij];

            // find diamonds that form the stingray's wings
            for (TypeIdx t_k = 0; t_k < g->nTypes; t_k++) {
                string key = to_string(t_k);
                // continue if there is no triangle of type t_k
                if (tris_e_ij.find(key) == tris_e_ij.end())
                    continue;
                Count triangles_k = tris_e_ij[key];
                if (triangles_k == 0)
                    continue;
                Count diamonds_2k = triangles_k * (triangles_k - 1) / 2;    // triangleCount choose 2

                // loop over tail types
                for (TypeIdx t_r = 0; t_r < g->nTypes; t_r++) {
                    Count deg_i_t_r = g->type_degree(v_i, t_r); // no. possible tails at i
                    Count deg_j_t_r = g->type_degree(v_j, t_r); // no. possible tails at j
                    if (deg_i_t_r == 0 and deg_j_t_r == 0)
                        continue;
                    if (t_r == t_i)
                        deg_j_t_r--;
                    if (t_r == t_j)
                        deg_i_t_r--;
                    if (t_r == t_k){
                        deg_i_t_r -= 2;
                        deg_j_t_r -= 2;
                    }
                    count = diamonds_2k * deg_i_t_r;  // tail is attached to i
                    types = {t_r, t_i, t_j, t_k, t_k};
                    ret.update("stingrays", types, count);

                    count = diamonds_2k * deg_j_t_r;  // tail is attached to j
                    types = {t_r, t_j, t_i, t_k, t_k};
                    ret.update("stingrays", types, count);

                    // consider case where triangles for stingray wings are of different types
                    for (TypeIdx t_l = t_k + 1; t_l < g->nTypes; t_l++) {
                        key = to_string(t_l);
                        if (tris_e_ij.find(key) == tris_e_ij.end())
                            continue;       // there is no triangle of type t_l
                        Count triangles_l = tris_e_ij[key];
                        if (triangles_l == 0)
                            continue;
                        Count diamonds_k_l = triangles_k * triangles_l;    // combine each k-triangle with each r-triangle
                        deg_i_t_r = g->type_degree(v_i, t_r);   // no. possible tails at i
                        deg_j_t_r = g->type_degree(v_j, t_r);   // no. possible tails at j
                        if (t_r == t_i)
                            deg_j_t_r--;
                        if (t_r == t_j)
                            deg_i_t_r--;
                        if (t_r == t_k){
                            deg_i_t_r--;
                            deg_j_t_r--;
                        }
                        if (t_r == t_l){
                            deg_i_t_r--;
                            deg_j_t_r--;
                        }
                        if (deg_i_t_r <= 0 and deg_j_t_r <= 0)
                            continue;
                        count = diamonds_k_l * deg_i_t_r;    // tail is attached to i
                        types = {t_r, t_i, t_j, t_k, t_l};
                        ret.update("stingrays", types, count);

                        if (deg_j_t_r <= 0)
                            continue;
                        count = diamonds_k_l * deg_j_t_r;    // tail is attached to j
                        types = {t_r, t_j, t_i, t_k, t_l};
                        ret.update("stingrays", types, count);
                    }
                }
            }
        }
    }
    return ret;
}
