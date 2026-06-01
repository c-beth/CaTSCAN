#include "FiveNode.hpp"


// Pattern 2: Prong
//
//         2
//        /|\
//       / | \
//      1  3  4
//      |
//      |
//      0
//
// Method:
//   Loop over all edges in the graph and set them as 1-2 in the graphlet.
//   Combine all the possibilities to choose two neighbors of node 2,
//   and one neighbor of node 1. Adjust the count for cases where n3 = n0,
//   or n4 = n0 by subtracting 2 counts for each tailed-tri instance.
//
// Arguments:
//   g: CSR of the graph
//   fourNodeInfo: Counts + Info of 4-node graphlets
//
// Return:
//   Total count of pattern 2 in the graph

GraphletInfo CaTSCAN::countProngs(CHIN *g, FourNodeInfo &fourNodeInfo) {

    GraphletInfo ret;
    Count count;
    TypeVector types;

    for (NodeIdx v_i = 0; v_i < g->nNodes; v_i++) {
        TypeIdx t_i = g->nodeTypes[v_i];

        for (EdgeIdx e_ij = g->offsets[v_i]; e_ij < g->offsets[v_i + 1]; e_ij++) {
            NodeIdx v_j = g->nbors[e_ij];
            TypeIdx t_j = g->nodeTypes[v_j];

            for (TypeIdx t_k = 0; t_k < g->nTypes; t_k++) {
                Count dof_i_tk = g->type_degree(v_i, t_k);
                if (t_k == t_j)
                    dof_i_tk--;     // remove node 2 from the nodes that can be chosen for node 0 if they are of the same type
                if (dof_i_tk == 0)
                    continue;

                for (TypeIdx t_r = 0; t_r < g->nTypes; t_r++) {
                    Count dof_j_tr = g->type_degree(v_j, t_r);
                    if (t_r == t_i)
                        dof_j_tr--;     // remove node 1 from the nodes that can be chosen for node 3 if they are of the same type
                    if (dof_j_tr == 0)
                        continue;
                    count = dof_i_tk * dof_j_tr * (dof_j_tr - 1) / 2;
                    types = {t_k, t_i, t_j, t_r, t_r};
                    ret.update("prong", types, count);      // update if the types for 3 and 4 are the same

                    for (TypeIdx t_l = t_r + 1; t_l < g->nTypes; t_l++) {
                        Count dof_j_ts = g->type_degree(v_j, t_l);
                        if (t_l == t_i)
                            dof_j_ts--;     // remove node 1 from the nodes that can be chosen for node 4 if they are of the same type
                        if (dof_j_ts == 0)
                            continue;
                        count = dof_i_tk * dof_j_tr * dof_j_ts;
                        types = {t_k, t_i, t_j, t_r, t_l};  // update if the types for 3 and 4 are distinct
                        ret.update("prong", types, count);
                    }
                }
            }
        }
    }

    // correction terms that account for over-counting
    CountMap &TT_i = fourNodeInfo.tailedTriangles.typedTotals;
    for (auto const & [type_hash, tt_count] :  TT_i) {
        TypeVector t = decodeHash(type_hash);
        TypeVector PRG_Types_1 = {t[2], t[3], t[0], min(t[1], t[2]), max(t[1], t[2])};
        TypeVector PRG_Types_2 = {t[3], t[2], t[0], min(t[1], t[3]), max(t[1], t[3])};
        ret.update("prong", PRG_Types_1, -tt_count);
        ret.update("prong", PRG_Types_2, -tt_count);
    }
    return ret;
}
