#include "FiveNode.hpp"


// Pattern 3: 4-Path
//
//
//      0----1----2----3----4
//
// Method:
//   Get two-paths for each node by expanding with an edge and expanding once again from the target node.
//   Combine combinations of two-paths to four path.
//   Account for cases where nodes 1 and 3 are identical by tracking squared degrees for the first edge expansions.
//   Cases where 0 is identical to 4 lead to 4-cycles:
//      - (0,3) or (1,4) are identical leads to tailed triangles
//      - (0,3) and (1,4) are identical leads to triangles
//   We apply corrections for these cases.
//
// Arguments:
//    g: CSR of the graph
//    threeNodeInfo: Counts + Info of 3-node graphlets
//    fourNodeInfo: Counts + Info of 4-node graphlets
//
// Return:
//   Total count of pattern 3 in the graph

GraphletInfo CaTSCAN::countFourPaths(CHIN *g, ThreeNodeInfo &threeNodeInfo, FourNodeInfo &fourNodeInfo) {
    GraphletInfo ret;
    Count count;

    for (NodeIdx v_i = 0; v_i < g->nNodes; v_i++) {
        TypeIdx t_i = g->nodeTypes[v_i];
        CountMap wedges_i;          // holds for each node types t_j t_k the number f wedges that v_i has
        CountMap wedges_i_sqr;      // holds for each node types t_k, t_l, t_j the number of combinations of 2 wedges that share the same one-hop neighbor of i i.e. (k1 <-> j <-> i, i <-> j <-> k2)

        for (EdgeIdx e_ij = g->offsets[v_i]; e_ij < g->offsets[v_i + 1]; e_ij++) {
            NodeIdx v_j = g->nbors[e_ij];
            TypeIdx t_j = g->nodeTypes[v_j];

            for (TypeIdx t_k = 0; t_k < g->nTypes; t_k++) {
                Count deg_jk = g->type_degree(v_j, t_k);
                string w_type_string = prettyPrintHash({t_j, t_k});
                string ws_type_string = prettyPrintHash({t_j, t_k, t_k});

                if (t_i == t_k) {
                    wedges_i[w_type_string] = wedges_i[w_type_string] + deg_jk - 1;
                    wedges_i_sqr[ws_type_string] = wedges_i_sqr[ws_type_string] + (deg_jk - 1) * (deg_jk - 1);
                } else {
                    wedges_i[w_type_string] += deg_jk;
                    wedges_i_sqr[ws_type_string] = wedges_i_sqr[ws_type_string] + (deg_jk) * (deg_jk);
                }

                for (TypeIdx t_r = t_k + 1; t_r < g->nTypes; t_r++) {
                    ws_type_string = prettyPrintHash({t_j, t_k, t_r});
                    Count deg_jr = g->type_degree(v_j, t_r);

                    if (t_i == t_k)
                        wedges_i_sqr[ws_type_string] += (deg_jk - 1) * deg_jr;
                    else if (t_i == t_r)
                        wedges_i_sqr[ws_type_string] += deg_jk * (deg_jr - 1);
                    else
                        wedges_i_sqr[ws_type_string] += deg_jk * deg_jr;
                }
            }
        }

        for (auto& [t_str1, cnt1] : wedges_i) {
            if (cnt1 == 0)
                continue;
            TypeIdx t_j1 = decodeHash(t_str1)[0];
            TypeIdx t_k1 = decodeHash(t_str1)[1];

            for (auto& [t_str2, cnt2] : wedges_i) {
                if (cnt2 == 0)
                    continue;
                if (t_str1 > t_str2)
                    continue;

                TypeIdx t_j2 = decodeHash(t_str2)[0];
                TypeIdx t_k2 = decodeHash(t_str2)[1];
                string ws_t_str = prettyPrintHash({t_j1, t_k1, t_k2});

                count = cnt1 * cnt2;
                if (t_str1 == t_str2)
                    count = (count - wedges_i_sqr[ws_t_str]) / 2;
                else if (t_j1 == t_j2)
                    count -= wedges_i_sqr[ws_t_str];

                TypeVector types = {t_k1, t_j1, t_i, t_j2, t_k2};
                ret.update("4-path", types, count);
            }
        }
    }

    // Apply corrections for over-counting
    CountMap &FourCycles = fourNodeInfo.fourCycles.typedTotals;
    for (auto const & [type_hash, four_cycle_count] : FourCycles) {
        TypeVector t = decodeHash(type_hash);
        TypeVector FP_types_1 = {t[0], t[1], t[2], t[3], t[0]};
        TypeVector FP_types_2 = {t[1], t[2], t[3], t[0], t[1]};
        TypeVector FP_types_3 = {t[2], t[3], t[0], t[1], t[2]};
        TypeVector FP_types_4 = {t[3], t[0], t[1], t[2], t[3]};
        ret.update("4-path",FP_types_1, -four_cycle_count);
        ret.update("4-path",FP_types_2, -four_cycle_count);
        ret.update("4-path",FP_types_3, -four_cycle_count);
        ret.update("4-path",FP_types_4, -four_cycle_count);
    }

    CountMap &TailedTris = fourNodeInfo.tailedTriangles.typedTotals;
    for (auto const & [type_hash, tailed_tris_count] : TailedTris) {
        TypeVector t = decodeHash(type_hash);
        TypeVector TT_types_1 = {t[1], t[0], t[2], t[3], t[0]};
        TypeVector TT_types_2 = {t[1], t[0], t[3], t[2], t[0]};
        ret.update("4-path",TT_types_1, -tailed_tris_count);
        ret.update("4-path",TT_types_2, -tailed_tris_count);
    }

    CountMap &Triangles = threeNodeInfo.triangles.typedTotals;
    for (auto const & [type_hash, triangle_count] : Triangles) {
        TypeVector t = decodeHash(type_hash);
        TypeVector T_types_1 = {t[0], t[1], t[2], t[0], t[1]};
        TypeVector T_types_2 = {t[1], t[2], t[0], t[1], t[2]};
        TypeVector T_types_3 = {t[2], t[0], t[1], t[2], t[0]};
        ret.update("4-path",T_types_1, -triangle_count);
        ret.update("4-path",T_types_2, -triangle_count);
        ret.update("4-path",T_types_3, -triangle_count);
    }

    return ret;
}
