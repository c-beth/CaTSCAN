#include "FourNode.hpp"


//  1    2
//  |  /
//  | /
//  0----3
//
GraphletInfo CaTSCAN::countThreeStars(CHIN *g) {
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
            count = deg_i_j * (deg_i_j - 1) * (deg_i_j - 2) / 6;    // deg_i_j choose 3
            types = {t_i, t_j, t_j, t_j};
            ret.update("3-star", types, count);

            for (TypeIdx t_k = t_j + 1; t_k < g->nTypes; t_k++) {
                Count deg_i_k = g->type_degree(v_i, t_k);
                if (deg_i_k == 0)
                    continue;
                count = (deg_i_j * (deg_i_j - 1) / 2) * deg_i_k;    // deg_i_j choose 2 * deg_i_k choose 1
                types = {t_i, t_j, t_j, t_k};
                ret.update("3-star", types, count);

                count = (deg_i_k * (deg_i_k - 1) / 2) * deg_i_j;    // deg_i_k choose 2 * deg_i_j choose 1
                types = {t_i, t_j, t_k, t_k};
                ret.update("3-star", types, count);

                for (TypeIdx t_r = t_k + 1; t_r < g->nTypes; t_r++) {
                    Count deg_i_r = g->type_degree(v_i, t_r);
                    if (deg_i_r == 0)
                        continue;
                    count = deg_i_j * deg_i_k * deg_i_r;    // deg_i_j choose 1 * deg_i_k choose 1 * deg_i_r choose 1
                    types = {t_i, t_j, t_k, t_r};
                    ret.update("3-star", types, count);
                }
            }
        }
    }
    return ret;
}
