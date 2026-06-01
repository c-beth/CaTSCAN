#include "FiveNode.hpp"


// Pattern 21: Five-Clique
//
// Method:
//   Use enumeration of triangles for each edge to compute four cliques.
//   Use enumeration of four cliques for each triangle to compute five cliques.
//   Make use of out edges to count instances only once.
//
// Arguments:
//   gout: CSR (out-edges) of the graph
//
// Return:
//   Total count of pattern 21 in the graph

GraphletInfo CaTSCAN::countFiveCliques(CHIN *gout) {
    GraphletInfo ret;
    auto *tri_ends = new NodeIdx[gout->nNodes];
    auto *four_ends = new NodeIdx[gout->nNodes];

    for (NodeIdx v_i = 0; v_i < gout->nNodes; v_i++) {
        TypeIdx t_i = gout->nodeTypes[v_i];

        for (EdgeIdx e_ij = gout->offsets[v_i]; e_ij < gout->offsets[v_i + 1]; e_ij++) {
            NodeIdx v_j = gout->nbors[e_ij];
            TypeIdx t_j = gout->nodeTypes[v_j];
            Count count = 0;         // store the number of triangles on Edge i,j

            for (EdgeIdx e_ik = e_ij + 1; e_ik < gout->offsets[v_i + 1]; e_ik++) {
                NodeIdx v_k = gout->nbors[e_ik];
                if (gout->isEdge(v_j,v_k) or gout->isEdge(v_k, v_j)) {
                    tri_ends[count] = v_k;
                    count++;
                }
            }
            for (Count pos_k = 0; pos_k < count; pos_k++) {
                NodeIdx v_k = tri_ends[pos_k];
                TypeIdx t_k = gout->nodeTypes[v_k];
                Count count2 = 0;
                for (Count pos_r = pos_k + 1; pos_r < count; pos_r++) {
                    NodeIdx v_r = tri_ends[pos_r];
                    // check if a 4-clique is formed between (i, j, k1, k2)
                    if (gout->isEdge(v_k, v_r) or gout->isEdge(v_r, v_k)) {
                        four_ends[count2] = v_r;
                        count2++;
                    }
                }
                for (Count pos_r = 0; pos_r < count2; pos_r++) {
                    NodeIdx v_r = four_ends[pos_r];
                    TypeIdx t_r = gout->nodeTypes[v_r];
                    for (Count pos_l = pos_r + 1; pos_l < count2; pos_l++) {
                        NodeIdx v_l = four_ends[pos_l];
                        TypeIdx t_l = gout->nodeTypes[pos_r];
                        if (gout->isEdge(v_r, v_l) or gout->isEdge(v_l, v_r)) {
                            TypeVector types = {t_i, t_j, t_k, t_r, t_l };
                            ret.update("5-clique", types, 1);
                        }
                    }
                }
            }
        }
    }
    delete[] tri_ends;
    delete[] four_ends;
    return ret;
}
