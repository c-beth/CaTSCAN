#include "FiveNode.hpp"


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
// Method:
//   Loop over all Edges of the graph as Edge 0-1.
//   Combine the nr. of triangles that Edge 0-1 participates in with
//   the possibilities to choose a neighbor of node 0 and a neighbor of node 1.
//   Adjust the count for the case that n3 = n4 by subtracting
//   2 counts for each diamond instance.
//
// Arguments:
//   g: CSR of the graph
//   gout: CSR (out-edges) of the graph
//   fourNodeInfo: Counts + Info of 4-node graphlets
//   threeNodeInfo: Counts + Info of 3-node graphlets
//
// Return:
//   Total count of pattern 6 in the graph

GraphletInfo CaTSCAN::countDoubleTailedTriangles(CHIN *g,
                                                 CHIN *gout,
                                                 FourNodeInfo& fourNodeInfo,
                                                 ThreeNodeInfo& threeNodeInfo) {

    GraphletInfo ret;
    TypeVector types;
    Count count, triangle_count, dof_i_tk, dof_j_tm;

    for (NodeIdx v_i = 0; v_i < gout->nNodes; v_i++) {
        TypeIdx t_i = gout->nodeTypes[v_i];

        for (EdgeIdx e_ij = gout->offsets[v_i]; e_ij < gout->offsets[v_i + 1]; e_ij++) {
            NodeIdx v_j = gout->nbors[e_ij];
            TypeIdx t_j = gout->nodeTypes[v_j];
            CountMap &tris_e_ij = threeNodeInfo.trianglesPerEdge[e_ij];
            if (tris_e_ij.empty())
                continue;

            for (auto & tri: tris_e_ij) {
                TypeIdx third_type = decodeNodeTriangle(tri.first)[0];
                triangle_count = tri.second;
                if (triangle_count == 0)
                    continue;

                for (TypeIdx t_k = 0; t_k < gout->nTypes; t_k++) {
                    dof_i_tk = g->type_degree(v_i, t_k);  // neighbors of i with type T_k
                    if (t_k == t_j)
                        dof_i_tk--;
                    if (t_k == third_type)
                        dof_i_tk--;
                    if (dof_i_tk == 0)
                        continue;

                    for (TypeIdx t_r = 0; t_r < gout->nTypes; t_r++) {
                        dof_j_tm = g->type_degree(v_j, t_r);  // neighbors of j with type t_r
                        if (t_r == t_i)
                            dof_j_tm--;
                        if (t_r == third_type)
                            dof_j_tm--;
                        if (dof_j_tm == 0)
                            continue;

                        count = triangle_count * dof_i_tk * dof_j_tm;
                        types = {t_i, t_j, third_type, t_k, t_r};
                        ret.update("double-tailed-triangle", types, count);
                    }
                }
            }
        }
    }

    // Apply corrections for over-counting
    CountMap &Diamonds = fourNodeInfo.diamonds.typedTotals;
    for (auto const & [type_hash, diamond_count] : Diamonds) {
        TypeVector t = decodeHash(type_hash);
        TypeVector DTT_types_1 = {t[0], t[1], t[2], t[3], t[3]};
        TypeVector DTT_types_2 = {t[0], t[1], t[3], t[2], t[2]};
        ret.update("double-tailed-triangle", DTT_types_1, -diamond_count);
        ret.update("double-tailed-triangle", DTT_types_2, -diamond_count);
    }
    return ret;
}
