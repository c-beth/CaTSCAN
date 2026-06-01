#include "FiveNode.hpp"


// Pattern 10: Cobra:
//
//         2
//        /| \
//       / |  \
//      4  |  1----0
//       \ |  /
//        \| /
//         3
//
// Method:
//   For each edge loop over triangle list to get triangle node.
//   Combine number of tails with triangles on edge n2 n3.
//
// Arguments:
//   g: CSR of the graph
//   gout: CSR (out-edges) of the graph
//   TriangleList: List of triangle nodes for each edge
//   fourNodeInfo: Counts + Info of 4-node graphlets
//   threeNodeInfo: Counts + Info of 3-node graphlets
//
// Return:
//   Total count of pattern 10 in the graph

GraphletInfo CaTSCAN::countCobras(CHIN *g,
                                  CHIN *gout,
                                  TriangleList tlist,
                                  FourNodeInfo& fourNodeInfo,
                                  ThreeNodeInfo& threeNodeInfo) {

    GraphletInfo ret;
    Count count;

    for (NodeIdx v_i = 0; v_i < g->nNodes; v_i++) {
        TypeIdx t_i = g->nodeTypes[v_i];
        Count deg_i = g->degree(v_i);

        for (EdgeIdx e_ij = g->offsets[v_i]; e_ij < g->offsets[v_i + 1]; e_ij++) {
            NodeIdx v_j = g->nbors[e_ij];
            TypeIdx t_j = g->nodeTypes[v_j];
            Count deg_j = g->degree(v_j);
            if (deg_j < deg_i or (deg_j == deg_i and v_j <= v_i))
                continue;
            EdgeIdx out_edge = gout->getEdge(v_i,v_j);
            CountMap tris_e_ij = threeNodeInfo.trianglesPerEdge[out_edge];

            for (Count pos = tlist.trioffsets[e_ij]; pos < tlist.trioffsets[e_ij + 1]; pos++) {
                NodeIdx tri_node  = tlist.triangles[pos];
                TypeIdx tri_type = g->nodeTypes[tri_node];

                for (auto & tri: tris_e_ij) {
                    TypeIdx tip_type = decodeNodeTriangle(tri.first)[0];
                    Count num_tris = tri.second;
                    if (tip_type == tri_type)
                        num_tris--;
                    if (num_tris < 1)
                        continue;
                    for (TypeIdx t_tail = 0; t_tail < g->nTypes; t_tail++) {
                        Count dof_tail = g->type_degree(tri_node, t_tail);
                        if (t_tail == t_i)
                            dof_tail--;
                        if (t_tail == t_j)
                            dof_tail--;
                        if (dof_tail == 0)
                            continue;
                        count =  num_tris * dof_tail;
                        TypeVector types = {t_tail, tri_type, t_i, t_j, tip_type};
                        ret.update("cobra", types, count);
                    }
                }
            }
        }
    }

    // Apply correction for over-counting
    CountMap &FourCliques = fourNodeInfo.fourCliques.typedTotals;
    for (auto const& [type_hash, clique_count] : FourCliques) {
        TypeVector t = decodeHash(type_hash);
        TypeVector C_types_1 = {t[0], t[1], t[2], t[3], t[0]};
        TypeVector C_types_2 = {t[0], t[2], t[1], t[3], t[0]};
        TypeVector C_types_3 = {t[0], t[3], t[1], t[2], t[0]};

        TypeVector C_types_4 = {t[1], t[0], t[2], t[3], t[1]};
        TypeVector C_types_5 = {t[1], t[2], t[0], t[3], t[1]};
        TypeVector C_types_6 = {t[1], t[3], t[0], t[2], t[1]};

        TypeVector C_types_7 = {t[2], t[0], t[1], t[3], t[2]};
        TypeVector C_types_8 = {t[2], t[1], t[0], t[3], t[2]};
        TypeVector C_types_9 = {t[2], t[3], t[0], t[1], t[2]};

        TypeVector C_types_10 = {t[3], t[0], t[1], t[2], t[3]};
        TypeVector C_types_11 = {t[3], t[1], t[0], t[2], t[3]};
        TypeVector C_types_12 = {t[3], t[2], t[0], t[1], t[3]};

        ret.update("cobra", C_types_1, -clique_count);
        ret.update("cobra", C_types_2, -clique_count);
        ret.update("cobra", C_types_3, -clique_count);
        ret.update("cobra", C_types_4, -clique_count);
        ret.update("cobra", C_types_5, -clique_count);
        ret.update("cobra", C_types_6, -clique_count);
        ret.update("cobra", C_types_7, -clique_count);
        ret.update("cobra", C_types_8, -clique_count);
        ret.update("cobra", C_types_9, -clique_count);
        ret.update("cobra", C_types_10, -clique_count);
        ret.update("cobra", C_types_11, -clique_count);
        ret.update("cobra", C_types_12, -clique_count);
    }
    return ret;
}
