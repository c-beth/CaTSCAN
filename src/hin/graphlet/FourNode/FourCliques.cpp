#include "FourNode.hpp"


//  1----2
//  | \/ |
//  | /\ |
//  0----3
//
GraphletInfo CaTSCAN::countFourCliques(CHIN *gout) {
    GraphletInfo ret;
    auto tri_ends = new NodeIdx[gout->nNodes + 1];

    for (NodeIdx v_i = 0; v_i < gout->nNodes; v_i++) {
        TypeIdx t_i = gout->nodeTypes[v_i];

        for (EdgeIdx e_ij = gout->offsets[v_i]; e_ij < gout->offsets[v_i + 1]; e_ij++) {
            NodeIdx v_j = gout->nbors[e_ij];
            TypeIdx t_j = gout->nodeTypes[v_j];
            Count count = 0;

            for (EdgeIdx e_ik = e_ij + 1; e_ik < gout->offsets[v_i + 1]; e_ik++) {
                NodeIdx v_k = gout->nbors[e_ik];
                if (gout->isEdge(v_j, v_k) or gout->isEdge(v_k, v_j)) {
                    tri_ends[count] = v_k;
                    count++;
                }
            }

            for (Count pos_k = 0; pos_k < count; pos_k++) {
                NodeIdx v_k = tri_ends[pos_k];
                TypeIdx t_k = gout->nodeTypes[v_k];

                for (Count pos_r = pos_k + 1; pos_r < count; pos_r++) {
                    NodeIdx v_r = tri_ends[pos_r];
                    TypeIdx t_r = gout->nodeTypes[v_r];

                    if (gout->isEdge(v_k, v_r) or gout->isEdge(v_r, v_k)) {
                        TypeVector types = {t_i, t_j, t_k, t_r};
                        ret.update("4-clique", types, 1);
                    }
                }
            }
        }
    }
    delete[] tri_ends;
    return ret;
}
