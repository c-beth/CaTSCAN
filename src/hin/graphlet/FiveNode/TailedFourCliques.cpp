#include "FiveNode.hpp"


// Pattern 15: Tailed-Four-Clique
//
//      0
//      |
//      |
//      1----3
//      | \/ |
//      | /\ |
//      2----4
//
// Method:
//   Use enumeration of triangles on some edge to construct four cliques.
//   Use out edges to avoid counting instances multiple times (start with the smallest node).
//   Expand each node of clique with possible tails.
//
// Arguments:
//   g: CSR of the graph
//   gout: CSR (out-edges) of the graph
//
// Return:
//   Total count of pattern 15 in the graph

GraphletInfo CaTSCAN::countTailedFourCliques(CHIN *g, CHIN *gout) {

    GraphletInfo ret;
    auto *tri_ends = new NodeIdx[gout->nNodes];
    TypeVector types;
    Count count;

    for (NodeIdx v_i = 0; v_i < gout->nNodes; v_i++) {
        TypeIdx t_i = gout->nodeTypes[v_i];

        for (EdgeIdx e_ij = gout->offsets[v_i]; e_ij < gout->offsets[v_i + 1]; e_ij++) {
            NodeIdx v_j = gout->nbors[e_ij];
            TypeIdx t_j = gout->nodeTypes[v_j];
            count = 0;         // store the number of triangles on Edge i, j

            for (EdgeIdx e_ik = e_ij + 1; e_ik < gout->offsets[v_i + 1]; e_ik++) {
                NodeIdx v_k = gout->nbors[e_ik];
                if (gout->isEdge(v_j,v_k) or gout->isEdge(v_k,v_j)) {
                    tri_ends[count] = v_k;
                    count++;
                }
            }
            for (NodeIdx pos_k = 0; pos_k < count; pos_k++) {
                NodeIdx v_k = tri_ends[pos_k];
                TypeIdx t_k = gout->nodeTypes[v_k];

                for (NodeIdx pos_r = pos_k + 1; pos_r < count; pos_r++) {
                    NodeIdx v_r = tri_ends[pos_r];
                    TypeIdx t_r = gout->nodeTypes[v_r];

                    // check if a 4-clique is formed between (i, j, k, r)
                    if (gout->isEdge(v_k, v_r) or gout->isEdge(v_r, v_k)) {
                        for (TypeIdx t = 0; t < gout->nTypes; t++) {
                            Count dof_i_t = g->type_degree(v_i, t);
                            Count dof_j_t = g->type_degree(v_j, t);
                            Count dof_k_t = g->type_degree(v_k, t);
                            Count dof_r_t = g->type_degree(v_r, t);
                            if (t_i == t) {
                                dof_j_t--;
                                dof_k_t--;
                                dof_r_t--;
                            }
                            if (t_j == t) {
                                dof_i_t--;
                                dof_k_t--;
                                dof_r_t--;
                            }
                            if (t_k == t) {
                                dof_i_t--;
                                dof_j_t--;
                                dof_r_t--;
                            }
                            if (t_r == t) {
                                dof_i_t--;
                                dof_j_t--;
                                dof_k_t--;
                            }
                            if (dof_i_t > 0) {
                                types = {t, t_i, t_j, t_k, t_r};
                                ret.update("tailed-4-clique", types,dof_i_t);
                            }
                            if (dof_j_t > 0) {
                                types = {t, t_j, t_i, t_k, t_r};
                                ret.update("tailed-4-clique", types,dof_j_t);
                            }
                            if (dof_k_t > 0) {
                                types = {t, t_k, t_i, t_j, t_r};
                                ret.update("tailed-4-clique", types, dof_k_t);
                            }
                            if (dof_r_t > 0) {
                                types = {t, t_r, t_i, t_j, t_k};
                                ret.update("tailed-4-clique", types, dof_r_t);
                            }
                        }
                    }
                }
            }
        }
    }
    delete[] tri_ends;
    return ret;
}
