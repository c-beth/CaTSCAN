#include "FourNode.hpp"


//  1----2
//  |    |
//  |    |
//  0----3
//
GraphletInfo CaTSCAN::countFourCycles(CHIN *gout, CHIN *gin) {
    GraphletInfo ret;
    NodeIdx v_i;
    EdgeIdx e_ij, e_jk;
    Count count, wc_r, wc_l;
    TypeVector types;

    auto wedge_count = new Count[gout->nTypes * gout->nNodes];      // stores number of wedges ending at a vertex
    for (NodeIdx v = 0; v < gout->nTypes * gout->nNodes; v++)       // initialize all wedge_count values to 0
        wedge_count[v] = 0;

    for (v_i = 0; v_i < gout->nNodes; v_i++) {
        TypeIdx t_i = gout->nodeTypes[v_i];

        for (e_ij = gin->offsets[v_i]; e_ij < gin->offsets[v_i + 1]; e_ij++) {
            NodeIdx v_j = gin->nbors[e_ij];
            TypeIdx t_j = gin->nodeTypes[v_j];

            for (e_jk = gout->offsets[v_j]; e_jk < gout->offsets[v_j + 1]; e_jk++) {
                NodeIdx v_k = gout->nbors[e_jk];
                if (v_k >= v_i)
                    continue;
                wedge_count[v_k * gout->nTypes + t_j]++;
            }
            for (e_jk = gin->offsets[v_j]; e_jk < gin->offsets[v_j + 1]; e_jk++) {
                NodeIdx v_k = gin->nbors[e_jk];
                wedge_count[v_k * gout->nTypes + t_j]++;
            }
        }

        for (e_ij = gin->offsets[v_i]; e_ij < gin->offsets[v_i + 1]; e_ij++) {
            NodeIdx v_j = gin->nbors[e_ij];

            for (e_jk = gout->offsets[v_j]; e_jk < gout->offsets[v_j + 1]; e_jk++) {
                // i and k are connected by an outout wedge: i <-- j --> k
                NodeIdx v_k = gout->nbors[e_jk];
                TypeIdx t_k = gout->nodeTypes[v_k];

                for (TypeIdx t_r = 0; t_r < gout->nTypes; t_r++) {
                    // check type combinations that form wedges with i and k
                    wc_r = wedge_count[v_k * gout->nTypes + t_r];
                    if (wc_r == 0)
                        continue;
                    count = wc_r * (wc_r - 1) / 2;
                    types = {t_i, t_r, t_k, t_r};
                    ret.update("4-cycle", types, count);

                    for (TypeIdx t_l = t_r + 1; t_l < gout->nTypes; t_l++) {
                        wc_l = wedge_count[v_k * gout->nTypes + t_l];
                        count = wc_r * wc_l;
                        types = {t_i, t_r, t_k, t_l};
                        ret.update("4-cycle", types, count);
                    }
                    wedge_count[v_k * gout->nTypes + t_r] = 0;
                }
            }
            for (e_jk = gin->offsets[v_j]; e_jk < gin->offsets[v_j + 1]; e_jk++) {
                // i and k are connected by an inout wedge: i <-- j <-- k
                NodeIdx v_k = gin->nbors[e_jk];
                TypeIdx t_k = gin->nodeTypes[v_k];

                for (TypeIdx t_r = 0; t_r < gout->nTypes; t_r++) {
                    wc_r = wedge_count[v_k * gin->nTypes + t_r];
                    if (wc_r == 0)
                        continue;
                    count = wc_r * (wc_r - 1) / 2;
                    types = {t_i, t_r, t_k, t_r};
                    ret.update("4-cycle", types, count);

                    for (TypeIdx t_l = t_r + 1; t_l < gout->nTypes; t_l++) {
                        wc_l = wedge_count[v_k * gin->nTypes + t_l];
                        count = wc_r * wc_l;
                        types = {t_i, t_r, t_k, t_l};
                        ret.update("4-cycle", types, count);
                    }
                    wedge_count[v_k * gin->nTypes + t_r] = 0;
                }
            }
        }
    }
    delete[] wedge_count;
    return ret;
}
