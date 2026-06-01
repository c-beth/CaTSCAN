#include "FiveNode.hpp"


// Pattern 8: Five Cycle
//
//      3----4
//      |     \
//      |      0
//      |     /
//      2----1
//
// Method:
//   Use path decomposition of the cycle
//   Combine directed three-paths with directed two paths
//
// Arguments:
//   g: CSR of the graph
//   gout: CSR (out-edges) of the graph
//
// Return:
//   Total count of pattern 8 in the graph

GraphletInfo CaTSCAN::countFiveCycles(CHIN *gout, CHIN *gin) {
    GraphletInfo ret;
    Count count;
    auto *wedge_count = new Count[gin->nNodes * gin->nTypes];

    for (int i = 0; i < gin->nNodes * gin->nTypes; i++)
        wedge_count[i] = 0;

    for (NodeIdx v_i = 0; v_i < gin->nNodes; v_i++) {
        TypeIdx t_i = gin->nodeTypes[v_i];
        for (EdgeIdx e_ij = gin->offsets[v_i]; e_ij < gin->offsets[v_i + 1]; e_ij++) {
            NodeIdx v_j = gin->nbors[e_ij];
            TypeIdx t_j = gin->nodeTypes[v_j];
            for (EdgeIdx e_jk = gin->offsets[v_j]; e_jk < gin->offsets[v_j + 1]; e_jk++) {
                NodeIdx v_k = gin->nbors[e_jk];
                wedge_count[v_k * gin->nTypes + t_j]++;
            }
            for (EdgeIdx e_jk = gout->offsets[v_j]; e_jk < gout->offsets[v_j + 1]; e_jk++) {
                NodeIdx v_k = gout->nbors[e_jk];
                if (v_k == v_i)
                    continue;
                wedge_count[v_k * gout->nTypes + t_j]++;
            }
        }
        for (EdgeIdx e_ij = gout->offsets[v_i]; e_ij < gout->offsets[v_i + 1]; e_ij++) {
            NodeIdx v_j = gout->nbors[e_ij];
            TypeIdx t_j = gout->nodeTypes[v_j];
            for (EdgeIdx e_jk = gout->offsets[v_j]; e_jk < gout->offsets[v_j + 1]; e_jk++) {
                NodeIdx v_k = gout->nbors[e_jk];
                wedge_count[v_k * gout->nTypes + t_j]++;
            }
        }

        for (EdgeIdx e_ij = gin->offsets[v_i]; e_ij < gin->offsets[v_i + 1]; e_ij++) {
            NodeIdx v_j = gin->nbors[e_ij];
            TypeIdx t_j = gin->nodeTypes[v_j];

            for (EdgeIdx e_jk = gin->offsets[v_j]; e_jk < gin->offsets[v_j + 1]; e_jk++) {
                NodeIdx v_k = gin->nbors[e_jk];
                TypeIdx t_k = gin->nodeTypes[v_k];

                for (EdgeIdx e_kr = gout->offsets[v_k]; e_kr < gout->offsets[v_k + 1]; e_kr++) {
                    NodeIdx v_r = gout->nbors[e_kr];
                    TypeIdx t_r = gout->nodeTypes[v_r];
                    if (v_r == v_i or v_r == v_j)
                        continue;

                    if (gout->isEdge(v_i,v_k) or gout->isEdge(v_k,v_i))
                        ret.update("5-cycle", {t_i, t_j, t_k, t_r, t_k}, -1);
                    if (gout->isEdge(v_j, v_r) or gout->isEdge(v_r, v_j))
                        ret.update("5-cycle", {t_i, t_j, t_k, t_r, t_j}, -1);

                    for (TypeIdx t_0 = 0; t_0 < gout->nTypes; t_0++) {
                        count = wedge_count[v_r * gout->nTypes + t_0];
                        TypeVector types = {t_i, t_j, t_k, t_r, t_0};
                        ret.update("5-cycle", types, count);
                    }
                }
            }
        }
        for (EdgeIdx e_ij = gin->offsets[v_i]; e_ij < gin->offsets[v_i + 1]; e_ij++) {
            NodeIdx v_j = gin->nbors[e_ij];
            TypeIdx t_j = gin->nodeTypes[v_j];
            for (EdgeIdx e_jk = gin->offsets[v_j]; e_jk < gin->offsets[v_j + 1]; e_jk++) {
                NodeIdx v_k = gin->nbors[e_jk];
                wedge_count[v_k * gin->nTypes + t_j] = 0;
            }
            for (EdgeIdx e_jk = gout->offsets[v_j]; e_jk < gout->offsets[v_j + 1]; e_jk++) {
                NodeIdx v_k = gout->nbors[e_jk];
                wedge_count[v_k * gout->nTypes + t_j] = 0;
            }
        }
        for (EdgeIdx e_ij = gout->offsets[v_i]; e_ij < gout->offsets[v_i + 1]; e_ij++) {
            NodeIdx v_j = gout->nbors[e_ij];
            TypeIdx t_j = gout->nodeTypes[v_j];
            for (EdgeIdx e_jk = gout->offsets[v_j]; e_jk < gout->offsets[v_j + 1]; e_jk++) {
                NodeIdx v_k = gout->nbors[e_jk];
                wedge_count[v_k * gout->nTypes + t_j] = 0;
            }
        }
    }
    delete[] wedge_count;
    return ret;
}
