#include "FiveNode.hpp"


// Pattern 18: Wheel
//
//      3-----2
//      | \ / |
//      |  0  |
//      | / \ |
//      4-----1
//
// Method:
//   Use triangle list based approach to count.
//
// Arguments:
//   g: CSR of the graph
//   TriangleList: List of triangle nodes for each edge
//
// Return:
//   Total count of pattern 18 in the graph

GraphletInfo CaTSCAN::countWheels(CHIN *g, TriangleList *triList) {

    GraphletInfo ret;
    Count count;
    TypeVector types;
    auto *diamond_count = new NodeIdx[g->nNodes * g->nTypes];

    for (int i = 0; i < g->nNodes * g->nTypes; i++)
        diamond_count[i] = 0;

    for (NodeIdx v_i = 0; v_i < g->nNodes; v_i++) {
        TypeIdx t_i = g->nodeTypes[v_i];
        Count deg_i = g->degree(v_i);

        for (EdgeIdx e_ij = g->offsets[v_i]; e_ij < g->offsets[v_i + 1]; e_ij++) {
            NodeIdx v_j = g->nbors[e_ij];
            TypeIdx t_j = g->nodeTypes[v_j];
            Count deg_j = g->degree(v_j);

            EdgeIdx e_ij_ordered;   // either points from i to j, or from j to i, depending on degree ordering
            if (deg_j < deg_i or (deg_j == deg_i and v_j < v_i))
                e_ij_ordered = g->getEdge(v_j, v_i);     // points from j to i
            else
                e_ij_ordered = g->getEdge(v_i, v_j);    // points from i to j

            // iterate over all Triangles <i, j, k> to build diamond_count
            for (EdgeIdx pos_k = triList->trioffsets[e_ij_ordered]; pos_k < triList->trioffsets[e_ij_ordered + 1]; pos_k++ ) {
                NodeIdx v_k = triList->triangles[pos_k];
                TypeIdx t_k = g->nodeTypes[v_k];
                Count deg_k = g->degree(v_k);

                EdgeIdx e_jk_ordered;   // either points from j to k, or from k to j, depending on degree ordering
                if (deg_k < deg_j or (deg_k == deg_j and v_k < v_j))
                    e_jk_ordered = g->getEdge(v_k, v_j);    // points from k to j
                else
                    e_jk_ordered = g->getEdge(v_j, v_k);    // points from j to k

                // find index of i in trioffsets, store as mid and use to find node r in next loop
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

                // Iterate over all triangles <j, k, r> that form a diamond with <i, j, k>,
                // i.e., the edge <j, k> is shared between the triangles.
                // Start loop a mid to only consider each pair of triangles once
                for (EdgeIdx pos_r = mid + 1; pos_r < triList->trioffsets[e_jk_ordered + 1]; pos_r++) {
                    NodeIdx v_r = triList->triangles[pos_r];
                    if (v_i < v_k and v_i < v_r)
                        diamond_count[v_r * g->nTypes + t_k]++;
                }
            }

            // iterate over all Triangles <i, j, k> to build new diamonds to combine with diamonds in diamond_count
            for (EdgeIdx pos_k = triList->trioffsets[e_ij_ordered]; pos_k < triList->trioffsets[e_ij_ordered + 1]; pos_k++) {
                NodeIdx v_k = triList->triangles[pos_k];
                if (v_k <= v_i)
                    continue;
                Count deg_k = g->degree(v_k);
                EdgeIdx e_jk_ordered;   // either points from j to k, or from k to j, depending on degree ordering
                if (deg_k < deg_j or (deg_k == deg_j and v_k < v_j))
                    e_jk_ordered = g->getEdge(v_k, v_j);    // points from k to j
                else
                    e_jk_ordered = g->getEdge(v_j, v_k);    // points from j to k

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

                // Iterate over all triangles <j, k, r> that form a diamond with <i, j, k>,
                // i.e., the edge <j, k> is shared between the triangles.
                // Start loop a mid to only consider each pair of triangles once
                for (EdgeIdx pos_r = mid + 1; pos_r < triList->trioffsets[e_jk_ordered + 1]; pos_r++) {
                    NodeIdx v_r = triList->triangles[pos_r];
                    TypeIdx t_r = g->nodeTypes[v_r];
                    for (TypeIdx t_0 = 0; t_0 < g->nTypes; t_0++) {
                        Count dc_0 = diamond_count[v_r * g->nTypes + t_0];
                        if (dc_0 == 0)
                            continue;
                        count = dc_0 * (dc_0 - 1) / 2;
                        types = {t_j, t_i, t_0, t_r, t_0};
                        ret.update("wheel",types, count);
                        for (TypeIdx t_1 = t_0 + 1; t_1 < g->nTypes; t_1++) {
                            Count dc_1 = diamond_count[v_r * g->nTypes + t_1];
                            if (dc_1 == 0)
                                continue;
                            count = dc_0 * dc_1;
                            types = {t_j, t_i, t_0, t_r, t_1};
                            ret.update("wheel",types, count);
                        }
                        diamond_count[v_r * g->nTypes + t_0] = 0;
                    }
                }
            }
        }
    }
    delete[] diamond_count;
    return ret;
}
