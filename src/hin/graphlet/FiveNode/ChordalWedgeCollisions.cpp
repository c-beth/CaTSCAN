#include "FiveNode.hpp"


// Pattern 17: Chordal-Wedge collision
//
//         0
//        /  \
//       /    \
//      1      3
//      | \  / |
//      |  \/  |
//      |  /\  |
//      | /  \ |
//      2------4
//
// Method:
//   Fix node 1 and for all nodes k store the wedges that end in  k
//   For each edge that is connected
//   Combine the number of triangles sharing an edge with
//   the o-o/i-o wedges with the number of wedges to obtain the count
//   Adjust the count for the case that n2=n3 or n2=n4
//   2 counts for each diamond instance
//
// Arguments:
//   g: CSR of the graph
//   TriangleList: List of triangle nodes for each edge
//
// Return:
//   Total count of pattern 17 in the graph

GraphletInfo CaTSCAN::countChordalWedgeCollisions(CHIN *g, TriangleList *triList) {

    GraphletInfo ret;
    Count count;
    auto *wedge_count = new NodeIdx[g->nNodes * g->nTypes];

    for (NodeIdx i = 0; i < g->nNodes * g->nTypes; i++)
        wedge_count[i] = 0;

    for (NodeIdx v_i = 0; v_i < g->nNodes; v_i++) {
        Count deg_i = g->degree(v_i);
        TypeIdx t_i = g->nodeTypes[v_i];

        // find all wedges to populate the ends of the wedges
        for (EdgeIdx e_ij = g->offsets[v_i]; e_ij < g->offsets[v_i + 1]; e_ij++) {
            NodeIdx v_j = g->nbors[e_ij];
            TypeIdx t_j = g->nodeTypes[v_j];
            for (EdgeIdx e_jk = g->offsets[v_j]; e_jk < g->offsets[v_j + 1]; e_jk++) {
                NodeIdx v_k = g->nbors[e_jk];  // v_i -- v_j -- v_k is wedge centered at v_j
                if (v_k <= v_i)
                    continue;
                wedge_count[v_k * g->nTypes + t_j]++;   // for all counted wedges i < k, we either have out-out with k > i in-out or in-in
            }
        }

        for (EdgeIdx e_ij = g->offsets[v_i]; e_ij < g->offsets[v_i + 1]; e_ij++) {
            NodeIdx v_j = g->nbors[e_ij];
            TypeIdx t_j = g->nodeTypes[v_j];
            Count deg_j = g->degree(v_j);
            EdgeIdx e_ij_ordered;
            if (deg_j < deg_i or (deg_j == deg_i and v_j < v_i))
                e_ij_ordered = g->getEdge(v_j, v_i);
            else
                e_ij_ordered = g->getEdge(v_i, v_j);

            for (EdgeIdx tri_pos = triList->trioffsets[e_ij_ordered]; tri_pos < triList->trioffsets[e_ij_ordered + 1]; tri_pos++) {
                NodeIdx v_k = triList->triangles[tri_pos];
                TypeIdx t_k = g->nodeTypes[v_k];
                Count deg_k = g->degree(v_k);
                EdgeIdx e_jk_ordered;
                if (deg_k < deg_j or (deg_k == deg_j and v_k < v_j))
                    e_jk_ordered = g->getEdge(v_k, v_j);
                else
                    e_jk_ordered = g->getEdge(v_j, v_k);

                EdgeIdx low = triList->trioffsets[e_jk_ordered];
                EdgeIdx high = triList->trioffsets[e_jk_ordered + 1] - 1;
                EdgeIdx mid;
                while (low <= high) {
                    mid = (low + high) / 2;
                    if (triList->triangles[mid] == v_i)
                        break;
                    if (triList->triangles[mid] > v_i)
                        high = mid - 1;
                    else
                        low = mid + 1;
                }

                for (EdgeIdx next_tri_pos = mid + 1; next_tri_pos < triList->trioffsets[e_jk_ordered + 1]; next_tri_pos++) {
                    NodeIdx v_r = triList->triangles[next_tri_pos];
                    TypeIdx t_r = g->nodeTypes[v_r];

                    // v_j and v_k form the chord of the diamond, we now look for wedges that connect v_i and v_r
                    for (TypeIdx t = 0; t < g->nTypes; t++) {   // t is the type of the wedge center between v_i and v_r
                        count = wedge_count[v_r * g->nTypes + t];
                        if (t == t_j)
                            count--;
                        if (t == t_k)
                            count--;
                        TypeVector types = {t, t_i, t_j, t_r, t_k};
                        ret.update("chordal-wedge-collision", types, count);
                    }
                }
            }
        }

        for (EdgeIdx e_ij = g->offsets[v_i]; e_ij < g->offsets[v_i + 1]; e_ij++) {
            NodeIdx v_j = g->nbors[e_ij];
            TypeIdx t_j = g->nodeTypes[v_j];
            for (EdgeIdx e_jk = g->offsets[v_j]; e_jk < g->offsets[v_j + 1]; e_jk++) {
                NodeIdx v_k = g->nbors[e_jk];
                wedge_count[v_k * g->nTypes + t_j] = 0;
            }
        }
    }
    ret.total = ret.total / 2;
    for (auto& [type, typedCount] : ret.typedTotals)
        ret.typedTotals[type] = typedCount / 2;
    delete[] wedge_count;
    return ret;
}
