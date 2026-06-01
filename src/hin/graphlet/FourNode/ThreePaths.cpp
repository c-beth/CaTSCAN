#include "FourNode.hpp"


//  0    3
//  |    |
//  |    |
//  1----2
//
GraphletInfo CaTSCAN::countThreePaths(CHIN *g, CHIN *gout, ThreeNodeInfo &threeNodeInfo) {
    GraphletInfo ret;
    Count count;
    TypeVector types;
    for (NodeIdx v_i = 0; v_i < gout->nNodes; v_i++) {
        for (EdgeIdx e_ij = gout->offsets[v_i]; e_ij < gout->offsets[v_i + 1]; e_ij++) {
            NodeIdx v_j = gout->nbors[e_ij];
            TypeIdx t_i = g->nodeTypes[v_i];
            TypeIdx t_j = g->nodeTypes[v_j];
            for (TypeIdx t_k = 0; t_k < g->nTypes; t_k++) {
                Count deg_i_k = g->type_degree(v_i, t_k);
                if (t_k == t_j)
                    deg_i_k--;
                if (deg_i_k == 0)
                    continue;
                for (TypeIdx t_r = 0; t_r < g->nTypes; t_r++) {
                    Count deg_j_r = g->type_degree(v_j, t_r);
                    if (t_r == t_i)
                        deg_j_r--;
                    if (deg_j_r == 0)
                        continue;
                    count = deg_i_k * deg_j_r;
                    types = {t_k, t_i, t_j, t_r};
                    ret.update("3-path", types, count);
                }
            }
        }
    }

    // correction of over-counting due to triangles
    CountMap &tris = threeNodeInfo.triangles.typedTotals;
    for (auto const& [type_hash, triangle_count] : tris) {
        TypeVector t = decodeHash(type_hash);
        types = {t[0], t[1], t[2], t[0]};
        ret.update("3-path", types, -triangle_count);
        types = {t[1], t[0], t[2], t[1]};
        ret.update("3-path", types, -triangle_count);
        types = {t[2], t[0], t[1], t[2]};
        ret.update("3-path", types, -triangle_count);
    }
    return ret;
}
