#include "FiveNode.hpp"


// Pattern 5: Long-Tailed-Triangle
//
//                   4
//                  / \
//                 /   \
//      0----1----2-----3
//
// Method:
//   Loop over all Nodes of the graph as Node 2.
//   Combine the amount of triangles that node 2 participates in
//   with the number of two-paths starting from node 2 for all type combinations.
//   Subtract the cases where:
//      - n0 = n3 or n0 = n4 -> Diamond
//      - n1 = n3 or n1 = n4 -> Tailed-Triangle
//      - n0 = n3 and n1 = n4 (or vice versa) -> Triangle
//
// Arguments:
//   g: CSR of the graph
//   fourNodeInfo: Counts + Info of 4-node graphlets
//   threeNodeInfo: Counts + Info of 3-node graphlets
//
// Return:
//   Total count of pattern 5 in the graph

GraphletInfo CaTSCAN::countLongTailedTriangles(CHIN *g,
                                               FourNodeInfo& fourNodeInfo,
                                               ThreeNodeInfo& threeNodeInfo) {

    GraphletInfo ret;
    TypeVector types;
    Count count;

    for (NodeIdx v_i = 0; v_i < g->nNodes; v_i++) {
        TypeIdx t_i = g->nodeTypes[v_i];
        CountMap &tris_i = threeNodeInfo.trianglesPerNode[v_i];

        for (auto const & [type_hash, tri_count] :  tris_i) {
            if (tri_count == 0)
                continue;
            TypeVector triangleTypes = decodeNodeTriangle(type_hash);
            TypeIdx t_r = triangleTypes[0];
            TypeIdx t_l = triangleTypes[1];

            for (EdgeIdx e_ij = g->offsets[v_i]; e_ij < g->offsets[v_i + 1]; e_ij++) {
                NodeIdx v_j = g->nbors[e_ij];
                TypeIdx t_j = g->nodeTypes[v_j];

                for (TypeIdx t_k = 0; t_k < g->nTypes; t_k++) {
                    types = {t_k, t_j, t_i, t_r, t_l};
                    Count dof_j_tk = g->type_degree(v_j,t_k);
                    if (t_k == t_i)
                        dof_j_tk--;
                    if (dof_j_tk == 0)
                        continue;
                    count = tri_count * dof_j_tk;
                    ret.update("long-tailed-triangle", types, count);
                }
            }
        }
    }

    // Apply correction for over-counting. Occurs for diamonds, tailed triangles, and triangles.

    // For each of the nodes on the shared edge between the two triangles in the diamond there are two ways the diamond is expanded to an LTT
    CountMap &Diamonds = fourNodeInfo.diamonds.typedTotals;
    for (auto const & [type_hash, diamond_count] : Diamonds) {
        TypeVector t = decodeHash(type_hash);
        TypeVector LTT_types_1 = {t[0], t[2], t[1], t[0], t[3]};
        TypeVector LTT_types_2 = {t[0], t[3], t[1], t[0], t[2]};
        TypeVector LTT_types_3 = {t[1], t[2], t[0], t[1], t[3]};
        TypeVector LTT_types_4 = {t[1], t[3], t[0], t[1], t[2]};

        ret.update("long-tailed-triangle",LTT_types_1, -diamond_count);
        ret.update("long-tailed-triangle",LTT_types_2, -diamond_count);
        ret.update("long-tailed-triangle",LTT_types_3, -diamond_count);
        ret.update("long-tailed-triangle",LTT_types_4, -diamond_count);
    }

    // For each triangle node that is not the docking point for the tail the graphlet is expanded to an LTT
    CountMap &TailedTriangles = fourNodeInfo.tailedTriangles.typedTotals;
    for (auto const & [type_hash, tt_count] : TailedTriangles) {
        TypeVector t = decodeHash(type_hash);
        TypeVector LTT_types_1 = {t[1], t[0], t[2], t[0], t[3]};
        TypeVector LTT_types_2 = {t[1], t[0], t[3], t[0], t[2]};
        ret.update("long-tailed-triangle",LTT_types_1, -tt_count);
        ret.update("long-tailed-triangle",LTT_types_2, -tt_count);
    }

    CountMap &Triangles = threeNodeInfo.triangles.typedTotals;
    for (auto const & [type_hash, triangle_count] : Triangles) {
        TypeVector t = decodeHash(type_hash);
        TypeVector LTT_types_1 = {t[0], t[1], t[2], t[0], t[1]};
        TypeVector LTT_types_2 = {t[0], t[2], t[1], t[0], t[2]};
        TypeVector LTT_types_3 = {t[1], t[0], t[2], t[0], t[1]};
        TypeVector LTT_types_4 = {t[1], t[2], t[0], t[1], t[2]};
        TypeVector LTT_types_5 = {t[2], t[0], t[1], t[0], t[2]};
        TypeVector LTT_types_6 = {t[2], t[1], t[0], t[1], t[2]};

        ret.update("long-tailed-triangle",LTT_types_1, -triangle_count);
        ret.update("long-tailed-triangle",LTT_types_2, -triangle_count);
        ret.update("long-tailed-triangle",LTT_types_3, -triangle_count);
        ret.update("long-tailed-triangle",LTT_types_4, -triangle_count);
        ret.update("long-tailed-triangle",LTT_types_5, -triangle_count);
        ret.update("long-tailed-triangle",LTT_types_6, -triangle_count);
    }
    return ret;
}
