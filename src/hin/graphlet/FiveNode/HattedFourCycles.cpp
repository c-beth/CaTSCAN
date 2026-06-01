#include "FiveNode.hpp"


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
// Method:
//   Use decomposition into two wedges.
//   Combine out-out/in-out out-out/out-out in-out/in-out wedges to get cycles.
//   Count combine tail on all wedges with the number of wedges that share the same outer nodes.
//   Count the hats on each edge of the first wedge.
//   Correct for case that 0 matches node that does not appear in wedge (diamonds).
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

GraphletInfo CaTSCAN::countHattedFourCycles(CHIN *g,
                                            CHIN *gout,
                                            CHIN *gin,
                                            FourNodeInfo& fourNodeInfo,
                                            ThreeNodeInfo& threeNodeInfo) {

    GraphletInfo ret;
    auto *outout_count = new Count[gout->nNodes * gout->nTypes];
    auto *inout_count = new Count[gout->nNodes * gout->nTypes];
    Count count;
    TypeVector types;

    // initialize the wedge_count arrays with 0s
    for (NodeIdx v_i = 0; v_i < g->nNodes * g->nTypes; v_i++) {
        outout_count[v_i] = 0;
        inout_count[v_i] = 0;
    }

    for (NodeIdx v_i = 0; v_i < gin->nNodes; v_i++) {
        TypeIdx t_i = gin->nodeTypes[v_i];

        for (EdgeIdx e_ji = gin->offsets[v_i]; e_ji < gin->offsets[v_i + 1]; e_ji++) {
            NodeIdx v_j = gin->nbors[e_ji];
            TypeIdx t_j = gin->nodeTypes[v_j];

            // compute the out-out counts for wedge-center j with type t_j and wedge-end v_k
            for (EdgeIdx e_jk = gout->offsets[v_j]; e_jk < gout->offsets[v_j + 1]; e_jk++) {
                NodeIdx v_k = gout->nbors[e_jk];
                if (v_k == v_i)
                    continue;
                outout_count[v_k * g->nTypes + t_j]++;
            }

            // compute the in-out counts for wedge-center j with type t_j and wedge-end v_k
            for (EdgeIdx e_kj = gin->offsets[v_j]; e_kj < gin->offsets[v_j + 1]; e_kj++) {
                NodeIdx v_k = gin->nbors[e_kj];
                inout_count[v_k * g->nTypes + t_j]++;
            }
        }

        // iterate over all wedges again
        for (EdgeIdx e_ij = gin->offsets[v_i]; e_ij < gin->offsets[v_i + 1]; e_ij++) {
            NodeIdx v_j = gin->nbors[e_ij];
            TypeIdx t_j = gin->nodeTypes[v_j];
            CountMap tris_e_ij = threeNodeInfo.trianglesPerEdge[gout->getEdge(v_j, v_i)];

            for (EdgeIdx e_jk = gout->offsets[v_j]; e_jk < gout->offsets[v_j + 1]; e_jk++) {
                NodeIdx v_k = gout->nbors[e_jk];
                if (v_k == v_i)
                    continue;
                TypeIdx t_k = gout->nodeTypes[v_k];
                CountMap tris_e_jk = threeNodeInfo.trianglesPerEdge[e_jk];

                // iterate over all possible types for the second wedge to complete the cycle
                for (TypeIdx t_r = 0; t_r < g->nTypes; t_r++) {
                    Count outout_ik = outout_count[v_k * g->nTypes + t_r];
                    Count inout_ik = inout_count[v_k * g->nTypes + t_r];
                    if (t_j == t_r)
                        outout_ik--;
                    if (outout_ik == 0 and inout_ik == 0)
                        continue;

                    // iterate over all triangles on edge i_j and edge j_k to compute the counts
                    for (TypeIdx t_l = 0; t_l < g->nTypes; t_l++) {
                        string key = to_string(t_l);
                        if (tris_e_ij.find(key) == tris_e_ij.end())
                            continue;
                        Count triangles_ij_th = tris_e_ij[key];
                        if (triangles_ij_th == 0)
                            continue;
                        // Check if v_k >= v_i to avoid double counting
                        if (v_i < v_k) {
                            count = triangles_ij_th * outout_ik;
                            types = {t_l, t_j, t_k, t_i, t_r};
                            ret.update("hatted-4-cycle", types, count);
                        }
                        count = triangles_ij_th * inout_ik;
                        types = {t_l, t_j, t_k, t_i, t_r};
                        ret.update("hatted-4-cycle", types, count);
                    }

                    for (TypeIdx t_l = 0; t_l < g->nTypes; t_l++) {
                        string key = to_string(t_l);
                        if (tris_e_jk.find(key) == tris_e_jk.end())   // repeat the procedure for Triangles on edge e_jk
                            continue;
                        Count triangles_jk_th = tris_e_jk[key];
                        if (triangles_jk_th == 0)
                            continue;
                        if (v_i < v_k) {
                            count = triangles_jk_th * outout_ik;
                            types = {t_l, t_j, t_i, t_k, t_r};
                            ret.update("hatted-4-cycle", types, count);
                        }
                        count = triangles_jk_th * inout_ik;
                        types = {t_l, t_j, t_i, t_k, t_r};
                        ret.update("hatted-4-cycle", types, count);
                    }
                }
            }

            for (EdgeIdx e_jk = gin->offsets[v_j]; e_jk < gin->offsets[v_j + 1]; e_jk++) {
                NodeIdx v_k = gin->nbors[e_jk];
                TypeIdx t_k = gin->nodeTypes[v_k];
                CountMap tris_e_jk = threeNodeInfo.trianglesPerEdge[gout->getEdge(v_k, v_j)];

                // iterate over all possible types for the second wedge to complete the cycle
                for (TypeIdx t_r = 0; t_r < g->nTypes; t_r++) {
                    Count outout_ik = outout_count[v_k * g->nTypes + t_r];
                    Count inout_ik = inout_count[v_k * g->nTypes + t_r];
                    if (t_j == t_r)
                        inout_ik--;
                    if (outout_ik == 0 and inout_ik == 0)
                        continue;

                    // iterate over all triangles on edge i_j and edge j_k to compute the counts
                    for (TypeIdx t_l = 0; t_l < g->nTypes; t_l++) {
                        string key = to_string(t_l);
                        if (tris_e_ij.find(key) == tris_e_ij.end())
                            continue;
                        Count triangles_ij_th = tris_e_ij[key];
                        if (triangles_ij_th == 0)
                            continue;

                        count = triangles_ij_th * inout_ik;
                        types = {t_l, t_j, t_k, t_i, t_r};
                        ret.update("hatted-4-cycle",types, count);

                        count = triangles_ij_th * outout_ik;
                        types = {t_l, t_j, t_k, t_i, t_r};
                        ret.update("hatted-4-cycle",types, count);
                    }

                    // iterate over all triangles on edge i_j and edge j_k to compute the counts
                    for (TypeIdx t_l = 0; t_l < g->nTypes; t_l++) {
                        string key = to_string(t_l);
                        if (tris_e_jk.find(key) == tris_e_jk.end())
                            continue;
                        Count triangles_jk_th = tris_e_jk[key];
                        if (triangles_jk_th == 0)
                            continue;

                        count = triangles_jk_th * inout_ik;
                        types = {t_l, t_j, t_i, t_k, t_r};
                        ret.update("hatted-4-cycle", types, count);

                        count = triangles_jk_th * outout_ik;
                        types = {t_l, t_j, t_i, t_k, t_r};
                        ret.update("hatted-4-cycle", types, count);
                    }
                }
            }
        }

        // delete the counts
        for (EdgeIdx e_ij = gin->offsets[v_i]; e_ij < gin->offsets[v_i + 1]; e_ij++) {
            NodeIdx v_j = gin->nbors[e_ij];
            TypeIdx t_j = gin->nodeTypes[v_j];
            for (EdgeIdx e_jk = gout->offsets[v_j]; e_jk < gout->offsets[v_j + 1]; e_jk++) {
                NodeIdx v_k = gout->nbors[e_jk];
                if (v_k == v_i)
                    continue;
                outout_count[v_k * g->nTypes + t_j] = 0;
            }
            for (EdgeIdx e_jk = gin->offsets[v_j]; e_jk < gin->offsets[v_j + 1]; e_jk++) {
                NodeIdx v_k = gin->nbors[e_jk];
                inout_count[v_k * g->nTypes + t_j] = 0;
            }
        }
    }

    // Apply correction for over-counting
    CountMap &Diamonds = fourNodeInfo.diamonds.typedTotals;
    for (auto const& [type_hash, diamond_count] :  Diamonds) {
        TypeVector t = decodeHash(type_hash);
        TypeVector HFC_Types_03 = {t[1], t[0], t[2], t[3], t[1]};
        TypeVector HFC_Types_02 = {t[1], t[0], t[3], t[2], t[1]};
        TypeVector HFC_Types_13 = {t[0], t[1], t[3], t[2], t[0]};
        TypeVector HFC_Types_12 = {t[0], t[1], t[2], t[3], t[0]};
        ret.update("hatted-4-cycle",HFC_Types_03,-diamond_count);
        ret.update("hatted-4-cycle",HFC_Types_02,-diamond_count);
        ret.update("hatted-4-cycle",HFC_Types_13,-diamond_count);
        ret.update("hatted-4-cycle",HFC_Types_12,-diamond_count);
    }
    delete[] outout_count;
    delete[] inout_count;
    return ret;
}
