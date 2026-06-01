#include "FourNode.hpp"


//  3----1
//  |  / |
//  | /  |
//  0----2
//
GraphletInfo CaTSCAN::countDiamonds(CHIN *gout, ThreeNodeInfo &threeNodeInfo) {
    GraphletInfo ret;
    Count count;
    TypeVector types;
    for (NodeIdx v_i = 0; v_i < gout->nNodes; v_i++) {
        for (EdgeIdx e_ij = gout->offsets[v_i]; e_ij < gout->offsets[v_i + 1]; e_ij++) {
            NodeIdx v_j = gout->nbors[e_ij];
            TypeIdx t_i = gout->nodeTypes[v_i];
            TypeIdx t_j = gout->nodeTypes[v_j];
            CountMap &tris_e_ij = threeNodeInfo.trianglesPerEdge[e_ij];
            for (TypeIdx t_k = 0; t_k < gout->nTypes; t_k++) {
                string key = to_string(t_k);
                // continue if there is no triangle of type t_k
                if (tris_e_ij.find(key) == tris_e_ij.end())
                    continue;
                Count triangles = tris_e_ij[key];
                if (triangles == 0)
                    continue;
                count = triangles * (triangles - 1) / 2;    // triangleCount choose 2
                types = {t_i, t_j, t_k, t_k};
                ret.update("diamond", types, count);

                for (TypeIdx t_r = t_k + 1; t_r < gout->nTypes; t_r++) {
                    key = to_string(t_r);
                    // continue if there is no triangle of type t_r
                    if (tris_e_ij.find(key) == tris_e_ij.end())
                        continue;
                    Count triangles2 = tris_e_ij[key];
                    if (triangles2 == 0)
                        continue;
                    count = triangles * triangles2;    // triangleCount choose 2
                    types = {t_i, t_j, t_k, t_r};
                    ret.update("diamond", types, count);
                }
            }
        }
    }
    return ret;
}
