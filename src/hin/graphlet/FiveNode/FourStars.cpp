#include "FiveNode.hpp"


// Pattern 1: 4-star
//
//      1     2
//       \   /
//        \ /
//         0
//        / \
//       /   \
//      3     4
//
// Method:
//   Loop over all nodes in the graph as possible 4-star-Centers.
//   Go through all possible type Combinations for the 4 outer points (independent of ordering).
//
// Arguments:
//   g: CSR of the graph
//
// Return:
//   Total count of pattern 1 in the graph

GraphletInfo CaTSCAN::countFourStars(CHIN *g) {

    GraphletInfo ret;
    Count count;
    TypeVector types;

    for (NodeIdx v_i = 0; v_i < g->nNodes; v_i++) {      // loop over vertices
        // for each type of v_i consider all possible nodeTypes of v_j and v_k
        TypeIdx t_i = g->nodeTypes[v_i];
        for (TypeIdx t_j = 0; t_j < g->nTypes; t_j++) {
            Count deg_i_j = g->type_degree(v_i, t_j);
            if (deg_i_j == 0)
                continue;
            count = deg_i_j * (deg_i_j - 1) * (deg_i_j - 2) * (deg_i_j - 3) / 24;    // deg_i_j choose 4
            types = {t_i, t_j, t_j, t_j, t_j};
            ret.update("4-star", types, count);

            for (TypeIdx t_k = t_j + 1; t_k < g->nTypes; t_k++) {
                Count deg_i_k = g->type_degree(v_i, t_k);
                if (deg_i_k == 0)
                    continue;
                count = (deg_i_j * (deg_i_j - 1) * (deg_i_j - 2) / 6) * deg_i_k;    // deg_i_j choose 3 * deg_i_k choose 1
                types = {t_i, t_j, t_j, t_j, t_k};
                ret.update("4-star", types, count);

                count = (deg_i_k * (deg_i_k - 1) * (deg_i_k - 2) / 6) * deg_i_j;    // deg_i_k choose 3 * deg_i_j choose 1
                types = {t_i, t_j, t_k, t_k, t_k};
                ret.update("4-star", types, count);

                count = (deg_i_k * (deg_i_k - 1) / 2) * (deg_i_j * (deg_i_j - 1) / 2);    // deg_i_k choose 2 * deg_i_j choose 2
                types = {t_i, t_j, t_j, t_k, t_k};
                ret.update("4-star", types, count);

                for (TypeIdx t_r = t_k + 1; t_r < g->nTypes; t_r++) {
                    Count deg_i_r = g->type_degree(v_i, t_r);
                    if (deg_i_r == 0)
                        continue;
                    count = ((deg_i_j - 1) * deg_i_j / 2 ) * deg_i_k * deg_i_r;    // deg_i_j choose 2 * deg_i_k choose 1 * deg_i_r choose 1
                    types = {t_i, t_j, t_j, t_k, t_r};
                    ret.update("4-star", types, count);

                    count = ((deg_i_k - 1) * deg_i_k / 2 ) * deg_i_j * deg_i_r;    // deg_i_j choose 1 * deg_i_k choose 2 * deg_i_r choose 1
                    types = {t_i, t_j, t_k, t_k, t_r};
                    ret.update("4-star", types, count);

                    count = ((deg_i_r - 1) * deg_i_r / 2 ) * deg_i_k * deg_i_j;    // deg_i_j choose 1 * deg_i_k choose 1 * deg_i_r choose 2
                    types = {t_i, t_j, t_k, t_r, t_r};
                    ret.update("4-star", types, count);

                    for (TypeIdx t_l = t_r + 1; t_l < g->nTypes; t_l++) {
                        Count deg_i_l = g->type_degree(v_i, t_l);
                        if (deg_i_l == 0)
                            continue;
                        count = deg_i_l * deg_i_r * deg_i_k * deg_i_j;  // deg_i_j choose 1 * deg_i_k choose 1 * deg_i_r choose 1 * deg_i_l choose 1
                        types = {t_i, t_j, t_k, t_r, t_l};
                        ret.update("4-star", types, count);
                    }
                }
            }
        }
    }
    return ret;
}
