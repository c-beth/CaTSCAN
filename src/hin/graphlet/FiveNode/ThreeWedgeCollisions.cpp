#include "FiveNode.hpp"


// Pattern 13: Three-Wedge collision
//
//                .0-.
//               /  \ \
//              2    3 4
//               \  / /
//                `1-´
//
// Method:
//   Loop over all pairs of nodes without a direct edge of the graph as 0-1
//   Choose three distinct wedges that  0,1 participate in
//
// Arguments:
//    g: CSR of the graph
//
// Return:
//   Total count of pattern 13 in the graph

GraphletInfo CaTSCAN::countThreeWedgeCollisions(CHIN *g) {

    GraphletInfo ret;
    TypeVector types;
    Count count, wc1, wc2, wc3;
    auto *wedge_count = new NodeIdx[g->nNodes * g->nTypes];

    for (int i = 0; i < g->nNodes * g->nTypes; i++)
        wedge_count[i] = 0;

    for (NodeIdx v_i = 0; v_i < g->nNodes; v_i++) {
        TypeIdx t_i = g->nodeTypes[v_i];

        for (EdgeIdx e_ij = g->offsets[v_i]; e_ij < g->offsets[v_i + 1]; e_ij++) {
            NodeIdx v_j = g->nbors[e_ij];
            TypeIdx t_j = g->nodeTypes[v_j];
            for (EdgeIdx e_jk = g->offsets[v_j]; e_jk < g->offsets[v_j + 1]; e_jk++) {
                NodeIdx v_k = g->nbors[e_jk];
                if (v_k <= v_i)     // for all counted wedges where i < k at pos k, so we don't count wedges multiple times
                    continue;
                wedge_count[v_k * g->nTypes + t_j]++;
            }
        }

        for (EdgeIdx e_ij = g->offsets[v_i]; e_ij < g->offsets[v_i + 1]; e_ij++) {
            NodeIdx v_j = g->nbors[e_ij];   // j is current neighbor

            for (EdgeIdx e_jk = g->offsets[v_j]; e_jk < g->offsets[v_j + 1]; e_jk++) {
                NodeIdx v_k = g->nbors[e_jk];   // i <- j -> k is wedge centered at j
                TypeIdx t_k = g->nodeTypes[v_k];

                for (TypeIdx t_wc1 = 0; t_wc1 < g->nTypes; t_wc1++) {
                    wc1 = wedge_count[v_k * g->nTypes + t_wc1];
                    if (wc1 == 0)
                        continue;
                    count = wc1 * (wc1 - 1) * (wc1 - 2) / 6;    // wc1 choose 3
                    types = {t_i, t_k, t_wc1, t_wc1, t_wc1};
                    ret.update("3-wedge-collision", types, count);

                    for (TypeIdx t_wc2 = t_wc1 + 1; t_wc2 < g->nTypes; t_wc2++) {
                        wc2 = wedge_count[v_k * g->nTypes + t_wc2];
                        if (wc2 == 0)
                            continue;
                        count = wc1 * (wc1 - 1) / 2 * wc2;    // wc1 choose 2 * wc2 choose 1
                        types = {t_i, t_k, t_wc1, t_wc1, t_wc2};
                        ret.update("3-wedge-collision", types, count);

                        count = wc1 * wc2 * (wc2 - 1) / 2;  // wc1 choose 1 * wc2 choose 2
                        types = {t_i, t_k, t_wc1, t_wc2, t_wc2};
                        ret.update("3-wedge-collision", types, count);

                        for (TypeIdx t_wc3 = t_wc2 + 1; t_wc3 < g->nTypes; t_wc3++) {
                            wc3 = wedge_count[v_k * g->nTypes + t_wc3];
                            if (wc3 == 0)
                                continue;
                            count = wc1 * wc2 * wc3;    // wc1 choose 1 * wc2 choose 1 * wc3 choose 1
                            types = {t_i, t_k, t_wc1, t_wc2, t_wc3};
                            ret.update("3-wedge-collision", types, count);
                        }
                    }
                    wedge_count[v_k * g->nTypes + t_wc1] = 0;
                }
            }
        }
    }
    delete[] wedge_count;
    return ret;
}
