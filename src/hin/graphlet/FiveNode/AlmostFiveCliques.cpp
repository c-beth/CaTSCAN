#include "FiveNode.hpp"


// Pattern 20: Almost-five-Clique
//
// Method:
//   Use enumeration of triangles for an edge to compute four cliques
//   Use enumeration of four cliques and combine two four
//   Make use of out edges to count instances only once
//
// Arguments:
//   g: CSR of the graph
//
// Return:
//   Total count of pattern 20 in the graph

GraphletInfo CaTSCAN::countAlmostFiveCliques(CHIN *g) {
    GraphletInfo ret;
    auto *tri_ends = new NodeIdx[g->nEdges];
    auto *four_ends = new Count[g->nTypes];
    Count count, count_r, count_l;
    TypeVector types;

    for (NodeIdx v_i = 0; v_i < g->nNodes; v_i++) {
        TypeIdx t_i = g->nodeTypes[v_i];
        Count deg_i = g->degree(v_i);

        for (EdgeIdx e_ij = g->offsets[v_i]; e_ij < g->offsets[v_i + 1]; e_ij++) {
            NodeIdx v_j = g->nbors[e_ij];
            TypeIdx t_j = g->nodeTypes[v_j];
            Count deg_j = g->degree(v_j);

            if (deg_j < deg_i or (deg_j == deg_i and v_j < v_i))
                continue;
            Count tri_end_count = 0;

            // Don't start loop at e_ij + 1 here (as in the other cases when initializing tri_list), since tri_ends are used multiple times.
            // While one pos should be used only once for k, it can be used multiple times as l.
            for (EdgeIdx e_ik = g->offsets[v_i]; e_ik < g->offsets[v_i + 1]; e_ik++) {
                NodeIdx v_k = g->nbors[e_ik];
                if (g->isEdge(v_j,v_k) or g->isEdge(v_k,v_j)) {
                    tri_ends[tri_end_count] = v_k;
                    tri_end_count++;
                }
            }
            for (Count pos_k = 0; pos_k < tri_end_count; pos_k++) {
                NodeIdx v_k = tri_ends[pos_k];
                TypeIdx t_k = g->nodeTypes[v_k];
                Count deg_k = g->degree(v_k);

                if (deg_j < deg_k or (deg_j == deg_k and v_j < v_k))
                    continue;
                if (deg_k < deg_i or (deg_k == deg_i and v_k < v_i))
                    continue;

                for (TypeIdx t = 0; t < g->nTypes; t++)
                    four_ends[t] = 0;

                for (NodeIdx pos_r = 0; pos_r < tri_end_count; pos_r++) {
                    NodeIdx v_r = tri_ends[pos_r];
                    TypeIdx t_r = g->nodeTypes[v_r];
                    if (g->isEdge(v_k, v_r) or g->isEdge(v_r, v_k))
                        four_ends[t_r]++;
                }
                for (TypeIdx t_r = 0; t_r < g->nTypes; t_r++) {
                    count_r = four_ends[t_r];
                    if (count_r == 0)
                        continue;
                    count = count_r * (count_r - 1) / 2;
                    types = {t_r, t_r, t_i, t_j, t_k};
                    ret.update("almost-5-clique", types, count);
                    for (TypeIdx t_l = t_r + 1; t_l < g->nTypes; t_l++) {
                        count_l = four_ends[t_l];
                        count = count_r * count_l;
                        types = {t_r, t_l, t_i, t_j, t_k};
                        ret.update("almost-5-clique", types, count);
                    }
                }
            }
        }
    }
    delete[] tri_ends;
    delete[] four_ends;
    return ret;
}
