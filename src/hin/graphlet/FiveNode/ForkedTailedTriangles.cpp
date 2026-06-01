#include "FiveNode.hpp"


// Pattern 4: Forked-Tailed-Triangle
//
//         2
//        / \
//       /   \
//      0-----1
//      |\
//      | \
//      3  4
//
// Method:
//   Loop over all nodes of the graph as node 0.
//   Combine the nr. of triangles that node 0 participates in with
//   the possibilities to choose two distinct neighbors from node 0.
//
// Arguments:
//   g: CSR of the graph
//   threeNodeInfo: Counts + Info of 3-node graphlets
//
// Return:
//   Total count of pattern 4 in the graph

GraphletInfo CaTSCAN::countForkedTailedTriangles(CHIN *g, ThreeNodeInfo& threeNodeInfo) {
    GraphletInfo ret;
    TypeVector types;
    Count count;
    for (NodeIdx v_i = 0; v_i < g->nNodes; v_i++) {
        TypeIdx t_i = g->nodeTypes[v_i];
        CountMap &tris_i = threeNodeInfo.trianglesPerNode[v_i];

        for (auto & tri : tris_i) {    // loop over all triangles that node i is involved in
            Count triangles = tri.second;
            TypeVector triangleTypes = decodeNodeTriangle(tri.first);
            TypeIdx t_r = triangleTypes[0];
            TypeIdx t_l = triangleTypes[1];

            for (TypeIdx t_j = 0; t_j < g->nTypes; t_j++) {      // consider first tail type
                Count dof_i_tj = g->type_degree(v_i,t_j);
                if (t_j == t_r)
                    dof_i_tj--;     // one edge is already part of the triangle
                if (t_j == t_l)
                    dof_i_tj--;     // one edge is already part of the triangle
                if (dof_i_tj == 0)
                    continue;
                count = (dof_i_tj * (dof_i_tj - 1) / 2) * triangles;
                types = {t_i, t_r, t_l, t_j, t_j};
                ret.update("forked-tailed-triangle", types, count);

                for (TypeIdx t_k = t_j + 1; t_k < g->nTypes; t_k++) {    // consider second tail type
                    Count dof_i_tk = g->type_degree(v_i,t_k);
                    if (t_k == t_r)
                        dof_i_tk--;     // one edge is already part of the triangle
                    if (t_k == t_l)
                        dof_i_tk--;     // one edge is already part of the triangle
                    if (dof_i_tk == 0)
                        continue;
                    count = dof_i_tj * dof_i_tk * triangles;
                    types = {t_i, t_r, t_l, t_j, t_k};
                    ret.update("forked-tailed-triangle", types, count);
                }
            }
        }
    }
    return ret;
}
