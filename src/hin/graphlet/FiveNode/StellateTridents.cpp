#include "FiveNode.hpp"

// Pattern 14: Stellate Trident:
//
//               .0-.
//              / |\ \
//             2  | 3 4
//              \ |/ /
//               `1-´
//
// Method:
//   Loop over all edges of the graph as 0-1.
//   Choose three distinct triangles that edge 0-1 participates in.
//
// Arguments:
//   gout: CSR (out-edges) of the graph
//   threeNodeInfo: Counts + Info of 3-node graphlets
//
// Return:
//   Total count of pattern 14 in the graph

GraphletInfo CaTSCAN::countStellateTridents(CHIN *gout, ThreeNodeInfo& threeNodeInfo) {

    GraphletInfo ret;
    Count count;
    TypeVector types;

    for (NodeIdx v_i = 0; v_i < gout->nNodes; v_i++) {
        TypeIdx t_i = gout->nodeTypes[v_i];

        for (EdgeIdx e_ij = gout->offsets[v_i]; e_ij < gout->offsets[v_i + 1]; e_ij++) {
            NodeIdx v_j = gout->nbors[e_ij];
            TypeIdx t_j = gout->nodeTypes[v_j];
            CountMap &tris_e_ij = threeNodeInfo.trianglesPerEdge[e_ij];

            for (TypeIdx t_k = 0; t_k < gout->nTypes; t_k++) {
                string key_k = to_string(t_k);
                // continue if there is no triangle of type t_k
                if (tris_e_ij.find(key_k) == tris_e_ij.end())
                    continue;
                Count triangles = tris_e_ij[key_k];
                if (triangles > 2) {
                    count = triangles * (triangles - 1) * (triangles - 2) / 6;    // triangleCount choose 2
                    types = {t_i, t_j, t_k, t_k, t_k};
                    ret.update("stellate-trident", types, count);
                }

                for (TypeIdx t_r = t_k + 1; t_r < gout->nTypes; t_r++) {
                    string key_r = to_string(t_r);
                    // continue if there is no triangle of type t_r
                    if (tris_e_ij.find(key_r) == tris_e_ij.end())
                        continue;
                    Count triangles2 = tris_e_ij[key_r];
                    count = (triangles * (triangles - 1) / 2) * triangles2;    // Count for 2 times smaller type
                    types = {t_i, t_j, t_k, t_k, t_r};
                    ret.update("stellate-trident", types, count);
                    count = triangles * (triangles2 * (triangles2 - 1) / 2);    // Count for 2 times smaller type
                    types = {t_i, t_j, t_k, t_r, t_r};
                    ret.update("stellate-trident", types, count);

                    for (TypeIdx t_l = t_r + 1; t_l < gout->nTypes; t_l++) {
                        key_r = to_string(t_l);
                        if (tris_e_ij.find(key_r) == tris_e_ij.end())
                            continue;       // there is no triangle of type t_r
                        Count triangles3 = tris_e_ij[key_r];
                        count = triangles * triangles2 * triangles3;
                        types = {t_i, t_j, t_k, t_r, t_l};
                        ret.update("stellate-trident", types, count);
                    }
                }
            }
        }
    }
    return ret;
}
