#include "FourNode.hpp"


//  1    2
//  |  / |
//  | /  |
//  0----3
//
GraphletInfo CaTSCAN::countTailedTriangles(CHIN *g, CHIN *gout, ThreeNodeInfo &threeNodeInfo) {
    GraphletInfo ret;
    Count count;
    TypeVector types;

    for (NodeIdx v_i = 0; v_i < gout->nNodes; v_i++) {      // loop over vertices
        TypeIdx t_i = gout->nodeTypes[v_i];
        CountMap &tris_i = threeNodeInfo.trianglesPerNode[v_i];

        for (TypeIdx t_j = 0; t_j < gout->nTypes; t_j++) {      // v_j is the tail
            Count deg_i_j = g->type_degree(v_i, t_j);
            if (deg_i_j == 0)
                continue;

            for (auto & [type_hash, triangle_count] : tris_i) {
                TypeVector triangleTypes = decodeNodeTriangle(type_hash);
                Count tails = deg_i_j;
                TypeIdx t_k = triangleTypes[0];
                TypeIdx t_r = triangleTypes[1];
                if (t_k == t_j)
                    tails--;
                if (t_r == t_j)
                    tails--;
                count = tails * triangle_count;
                types = {t_i, t_j, t_k, t_r};
                ret.update("tailed-triangle", types, count);
            }
        }
    }
    return ret;
}
