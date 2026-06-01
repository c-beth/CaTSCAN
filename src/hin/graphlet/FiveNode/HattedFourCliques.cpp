#include "FiveNode.hpp"


// Pattern 19: Hatted-Four-Clique
//
//      3----2
//      |\  /|\
//      | \/ | 0
//      | /\ |/
//      4----1
//
// Method:
//   Use enumeration of triangles on some edge to construct four cliques.
//   Use out edges to avoid counting instances multiple times (start with the smallest node).
//   Expand each edge through the triangles subtract triangles that belong to the clique.
//
// Arguments:
//   gout: CSR (out-edges) of the graph
//   threeNodeInfo: Counts + Info of 3-node graphlets
//
// Return:
//   Total count of pattern 19 in the graph

GraphletInfo CaTSCAN::countHattedFourCliques(CHIN *gout, ThreeNodeInfo& threeNodeInfo) {

    GraphletInfo ret;
    auto *tri_ends = new NodeIdx[gout->nNodes];
    auto *ind_from_i = new EdgeIdx[gout->nNodes];
    auto *ind_from_j = new EdgeIdx[gout->nNodes];

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
                    ind_from_i[count] = e_ik;
                    ind_from_j[count] = gout->getEdgeUndirected(v_j, v_k);
                    count++;
                }
            }

            for (NodeIdx pos_k = 0; pos_k < count; pos_k++) {
                NodeIdx v_k = tri_ends[pos_k];
                TypeIdx t_k = gout->nodeTypes[v_k];

                for (NodeIdx pos_r = pos_k + 1; pos_r < count; pos_r++) {
                    NodeIdx v_r = tri_ends[pos_r];
                    TypeIdx t_r = gout->nodeTypes[v_r];

                    if (gout->isEdge(v_k, v_r) or gout->isEdge(v_r, v_k)) {
                        EdgeIdx e_kr = gout->getEdgeUndirected(v_k, v_r);

                        vector<EdgeIdx> four_clique_edges = {
                                e_ij,
                                ind_from_i[pos_k],
                                ind_from_i[pos_r],
                                ind_from_j[pos_k],
                                ind_from_j[pos_r],
                                e_kr
                        };

                        // The types that make up in the four clique sorted so that it can be combined with
                        // the hat type to the full type vector.
                        vector<TypeVector> four_clique_types = {{t_i, t_j, t_k, t_r},
                                                                {t_i, t_k, t_j, t_r},
                                                                {t_i, t_r, t_j, t_k},
                                                                {t_j, t_k, t_i, t_r},
                                                                {t_j, t_r, t_i, t_k},
                                                                {t_k, t_r, t_i, t_j}};

                        for (int i = 0; i < four_clique_edges.size(); i++) {
                            EdgeIdx e = four_clique_edges[i];
                            CountMap tris_e = threeNodeInfo.trianglesPerEdge[e];
                            for (auto& [t_str, cnt] : tris_e) {
                                TypeIdx t_hat = stoi(t_str);
                                if (cnt > 0) {
                                    TypeVector types = four_clique_types[i];
                                    types.insert(types.begin(),t_hat);
                                    ret.update("hatted-4-clique", types, cnt);
                                }
                            }
                        }

                        // Step 2: Subtract contributions from internal triangles
                        // Internal triangles in a 4-clique:
                        // (i,j,pos_k), (i,j,pos_r), (i,pos_k,pos_r), (j,pos_k,pos_r)
                        vector<TypeVector> triangle_nodes = {
                                {t_i, t_j, t_k},
                                {t_i, t_j, t_r},
                                {t_i, t_k, t_r},
                                {t_j, t_k, t_r}
                        };
                        // for each triangle_nodes hold node type that's not participating
                        TypeVector non_part_nodes = {t_r, t_k, t_j, t_i};
                        for (int m = 0; m < 4; ++m) {
                            for (int n = 0; n < 3; ++n) {
                                TypeIdx t_hat = triangle_nodes[m][n];
                                TypeIdx t_e1 = triangle_nodes[m][(n + 1) % 3];
                                TypeIdx t_e2 = triangle_nodes[m][(n + 2) % 3];
                                TypeVector types = {t_hat, t_e1, t_e2, non_part_nodes[m], t_hat};
                                ret.update("hatted-4-clique", types, -1);
                            }
                        }
                    }
                }
            }
        }
    }
    delete[] tri_ends;
    delete[] ind_from_i;
    delete[] ind_from_j;
    return ret;
}
